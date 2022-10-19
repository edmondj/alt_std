#pragma once

#include <type_traits>
#include <utility>

#if __cpp_exceptions

#include <exception>

namespace alt
{
  struct bad_function_call final : std::exception
  {
    inline virtual const char* what() const noexcept override
    {
      return "empty function was called";
    }
  };
}

#endif // __cpp_exceptions

namespace alt
{
  template<typename T>
  class move_only_function;

  template<typename T>
  class function;

  namespace functional_detail
  {
    template<typename TRet, typename... TArgs>
    struct move_only_funcs;

    template<typename TRet, typename... TArgs>
    struct funcs;
  }

  template<typename TRet, typename... TArgs>
  class move_only_function<TRet(TArgs...)>
  {
  public:
    move_only_function() noexcept = default;
    move_only_function(move_only_function&& other) noexcept;
    template<typename TFunc>
    move_only_function(TFunc&& func);

    move_only_function& operator=(move_only_function&& other) noexcept;
    template<typename TFunc>
    move_only_function& operator=(TFunc&& func);

    ~move_only_function();

    TRet operator()(TArgs...) const;

    explicit operator bool() const noexcept;

  protected:

    const functional_detail::move_only_funcs<TRet, TArgs...>* m_funcs = nullptr;
    void* m_data = nullptr;

    template<typename TFunc>
    static void* construct(TFunc&& func);
    void destruct() noexcept;

    move_only_function(const functional_detail::move_only_funcs<TRet, TArgs...>* funcs, void* data) noexcept;

    move_only_function(const move_only_function&) = delete;
    move_only_function(move_only_function&) = delete;
    move_only_function& operator=(const move_only_function&) = delete;
    move_only_function& operator=(move_only_function&) = delete;
  };

  template<typename TRet, typename... TArgs>
  class function<TRet(TArgs...)> : public move_only_function<TRet(TArgs...)>
  {
  private:
    using TBase = move_only_function<TRet(TArgs...)>;

  public:
    function() noexcept = default;
    function(function&& other) noexcept = default;
    template<typename TFunc, typename = std::enable_if_t<std::is_copy_constructible_v<TFunc>>>
    function(TFunc&& func);

    function& operator=(function&& other) noexcept = default;
    template<typename TFunc, typename = std::enable_if_t<std::is_copy_constructible_v<TFunc>>>
    function& operator=(TFunc&& func);

    function(const function& other);
    function(function&);

    function& operator=(const function& other);
    function& operator=(function&);
  };

  // Implementation

  namespace functional_detail
  {
    template<typename TFunc>
    using TRawFunc = std::remove_cv_t<std::remove_reference_t<TFunc>>;

    template<typename TFunc, typename TRet, typename... TArgs>
    static constexpr bool is_function_pointer_convertible = std::is_convertible_v<TRawFunc<TFunc>, TRet(*)(TArgs...)>;
  
    template<typename TRet, typename... TArgs>
    struct move_only_funcs
    {
      void (*destruct)(void*);
      TRet(*invoke)(void*, TArgs...);
    };

    template<typename TFunc, typename TRet, typename... TArgs>
    static constexpr move_only_funcs<TRet, TArgs...> move_only_funcs_for =
    {
      /*.destruct = */ [](void* data)
      {
        if constexpr (!is_function_pointer_convertible<TFunc, TRet, TArgs...>)
        {
          delete reinterpret_cast<TRawFunc<TFunc>*>(data);
        }
      },
      /*.invoke = */ [](void* data, TArgs... args) -> TRet
      {
        if constexpr (is_function_pointer_convertible<TFunc, TRet, TArgs...>)
        {
          return reinterpret_cast<TRet(*)(TArgs...)>(data)(args...);
        }
        else
        {
          return reinterpret_cast<TRawFunc<TFunc>*>(data)->operator()(args...);
        }
      }
    };

    template<typename TRet, typename... TArgs>
    struct funcs : move_only_funcs<TRet, TArgs...>
    {
      void* (*copy)(void*);
    };

    template<typename TFunc, typename TRet, typename... TArgs>
    static constexpr funcs<TRet, TArgs...> funcs_for =
    {
      /*.destruct = */ move_only_funcs_for<TFunc, TRet, TArgs...>.destruct,
      /*.invoke = */ move_only_funcs_for<TFunc, TRet, TArgs...>.invoke,
      /*.copy = */ [](void* data) -> void*
      {
        if constexpr (is_function_pointer_convertible<TFunc, TRet, TArgs...>)
        {
          return data;
        }
        else
        {
          return new TRawFunc<TFunc>(*reinterpret_cast<TRawFunc<TFunc>*>(data));
        }
      }
    };

  }

  template<typename TRet, typename ...TArgs>
  inline move_only_function<TRet(TArgs...)>::move_only_function(move_only_function&& other) noexcept
    : move_only_function(other.m_funcs, other.m_data)
  {
    other.m_funcs = nullptr;
    other.m_data = nullptr;
  }

  template<typename TRet, typename... TArgs>
  inline move_only_function<TRet(TArgs...)>::move_only_function(const functional_detail::move_only_funcs<TRet, TArgs...>* funcs, void* data) noexcept
    : m_funcs(funcs)
    , m_data(data)
  {}

  template<typename TRet, typename... TArgs>
  template<typename TFunc>
  inline move_only_function<TRet(TArgs...)>::move_only_function(TFunc&& func)
    : move_only_function(&functional_detail::move_only_funcs_for<TFunc, TRet, TArgs...>, construct(std::forward<TFunc>(func)))
  {}

  template<typename TRet, typename ...TArgs>
  inline auto move_only_function<TRet(TArgs...)>::operator=(move_only_function&& other) noexcept -> move_only_function&
  {
    destruct();
    m_funcs = other.m_funcs;
    other.m_funcs = nullptr;
    m_data = other.m_data;
    other.m_data = nullptr;
    return *this;
  }

  template<typename TRet, typename... TArgs>
  template<typename TFunc>
  inline auto move_only_function<TRet(TArgs...)>::operator=(TFunc&& func) -> move_only_function&
  {
    destruct();
    m_funcs = &functional_detail::move_only_funcs_for<TFunc, TRet, TArgs...>;
    m_data = construct(std::forward<TFunc>(func));
    return *this;
  }

  template<typename TRet, typename ...TArgs>
  inline move_only_function<TRet(TArgs...)>::~move_only_function()
  {
    destruct();
  }

  template<typename TRet, typename ...TArgs>
  inline void move_only_function<TRet(TArgs...)>::destruct() noexcept
  {
    if (m_funcs)
    {
      m_funcs->destruct(m_data);
    }
  }

  template<typename TRet, typename... TArgs>
  template<typename TFunc>
  void* move_only_function<TRet(TArgs...)>::construct(TFunc&& func)
  {
    if constexpr (functional_detail::is_function_pointer_convertible<TFunc, TRet, TArgs...>)
    {
      return reinterpret_cast<void*>(static_cast<TRet(*)(TArgs...)>(func));
    }
    else
    {
      return new functional_detail::TRawFunc<TFunc>(std::forward<TFunc>(func));
    }
  }

  template<typename TRet, typename... TArgs>
  TRet move_only_function<TRet(TArgs...)>::operator()(TArgs... args) const
  {
#if __cpp_exceptions
    if (!m_funcs)
    {
      throw bad_function_call{};
    }
#endif
    return m_funcs->invoke(m_data, args...);
  }

  template<typename TRet, typename... TArgs>
  move_only_function<TRet(TArgs...)>::operator bool() const noexcept
  {
    return m_data;
  }

  template<typename TRet, typename... TArgs>
  function<TRet(TArgs...)>::function(const function& other)
    : function()
  {
    operator=(other);
  }

  template<typename TRet, typename... TArgs>
  function<TRet(TArgs...)>::function(function& other)
    : function()
  {
    operator=(static_cast<const function&>(other));
  }

  template<typename TRet, typename... TArgs>
  auto function<TRet(TArgs...)>::operator=(const function& other) -> function&
  {
    TBase::destruct();
    if (other.m_data)
    {
      TBase::m_funcs = other.m_funcs;
      TBase::m_data = static_cast<const functional_detail::funcs<TRet, TArgs...>*>(TBase::m_funcs)->copy(other.m_data);
    }
    return *this;
  }

  template<typename TRet, typename... TArgs>
  auto function<TRet(TArgs...)>::operator=(function& other) -> function&
  {
    return operator=(static_cast<const function&>(other));
  }

  template<typename TRet, typename... TArgs>
  template<typename TFunc, typename>
  inline function<TRet(TArgs...)>::function(TFunc&& func)
    : TBase::move_only_function(&functional_detail::funcs_for<TFunc, TRet, TArgs...>, TBase::construct(std::forward<TFunc>(func)))
  {}

  template<typename TRet, typename... TArgs>
  template<typename TFunc, typename>
  inline auto function<TRet(TArgs...)>::operator=(TFunc&& func) -> function&
  {
    TBase::destruct();
    TBase::m_funcs = &functional_detail::funcs_for<TFunc, TRet, TArgs...>;
    TBase::m_data = TBase::construct(std::forward<TFunc>(func));
    return *this;
  }
}
