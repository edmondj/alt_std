#pragma once

#include <type_traits>
#include <cassert>
#include <utility>

namespace alt
{
  template<typename T>
  class move_only_function;

  template<typename T>
  class function;

  //namespace functional_detail
  //{
  //  template<typename TRet, typename... TArgs>
  //  struct move_only_ftable
  //  {
  //      void (*destruct)(void*);
  //      TRet(*invoke)(void*, TArgs...);
  //  };

  //  template<typename TRet, typename... TArgs>
  //  struct ftable : move_only_ftable<TRet, TArgs...>
  //  {
  //    void* copy(void*);
  //  };

  //  template<typename TFtable>
  //  struct function_data_base
  //  {
  //    void* data;
  //    const TFtable* ftable;

  //    function_data(void* data, const TFtable* ftable) noexcept
  //      : data(data), ftable(ftable)
  //    {}

  //    function_data(function_data&& other) noexcept
  //    {
  //      operator=(std::move(other));
  //    }

  //    function_data& operator=(function_data&& other) noexcept
  //    {
  //      std::swap(data, other.data);
  //      std::swap(ftable, other.ftable);
  //      return *this;
  //    }

  //    void destruct()
  //    {
  //      if (ftable)
  //      {
  //        ftable->destruct(data);
  //      }
  //    }

  //    ~function_data()
  //    {
  //      destruct();
  //    }
  //  };

  //  template<typename TRet, typename... TArgs>
  //  using move_only_function_data = function_data_base<move_only_ftable<TRet, TArgs...>>;

  //  template<typename TRet, typename... TArgs>
  //  struct function_data : function_data_base<ftable<TRet, TArgs...>>
  //  {
  //    using TBase = function_data_base<ftable<TRet, TArgs...>>;

  //    using TBase::function_data_base;
  //    using TBase::operator=;

  //    function_data(const function_data& other)
  //      : TBase(other.ftable->copy(other.data), other.ftable)
  //    {}

  //    function_data& operator=(const function_data& other)
  //    {
  //      destruct();

  //    }
  //  };
  //}

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
    template<typename TFunc>
    using TRawFunc = std::remove_cv_t<std::remove_reference_t<TFunc>>;

    struct move_only_funcs
    {
      void (*destruct)(void*);
      TRet(*invoke)(void*, TArgs...);
    };

    const move_only_funcs* m_funcs = nullptr;
    void* m_data;

    template<typename TFunc>
    static constexpr bool is_function_pointer_convertible = std::is_convertible_v<TRawFunc<TFunc>, TRet(*)(TArgs...)>;

    template<typename TFunc>
    static constexpr move_only_funcs move_only_funcs_for =
    {
      /*.destruct = */ [](void* data)
      {
        if constexpr (!is_function_pointer_convertible<TFunc>)
        {
          delete reinterpret_cast<TRawFunc<TFunc>*>(data);
        }
      },
      /*.invoke = */ [](void* data, TArgs... args) -> TRet
      {
        if constexpr (is_function_pointer_convertible<TFunc>)
        {
          return reinterpret_cast<TRet(*)(TArgs...)>(data)(args...);
        }
        else
        {
          return reinterpret_cast<TRawFunc<TFunc>*>(data)->operator()(args...);
        }
      }
    };

    template<typename TFunc>
    static void* construct(TFunc&& func);
    void destruct() noexcept;

    move_only_function(const move_only_funcs* funcs, void* data) noexcept;

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

  private:
    struct funcs : TBase::move_only_funcs
    {
      void* (*copy)(void*);
    };

    template<typename TFunc>
    static constexpr funcs funcs_for =
    {
      /*.destruct = */ TBase::template move_only_funcs_for<TFunc>.destruct,
      /*.invoke = */ TBase::template move_only_funcs_for<TFunc>.invoke,
      /*.copy = */ [](void* data) -> void*
      {
        if constexpr (TBase::template is_function_pointer_convertible<TFunc>)
        {
          return data;
        }
        else
        {
          return new typename TBase::template TRawFunc<TFunc>(*reinterpret_cast<typename TBase::template TRawFunc<TFunc>*>(data));
        }
      }
    };
  };

  // Implementation

  template<typename TRet, typename ...TArgs>
  inline move_only_function<TRet(TArgs...)>::move_only_function(move_only_function&& other) noexcept
    : move_only_function(other.m_funcs, other.m_data)
  {
    other.m_funcs = nullptr;
    other.m_data = nullptr;
  }

  template<typename TRet, typename... TArgs>
  inline move_only_function<TRet(TArgs...)>::move_only_function(const move_only_funcs* funcs, void* data) noexcept
    : m_funcs(funcs)
    , m_data(data)
  {}

  template<typename TRet, typename... TArgs>
  template<typename TFunc>
  inline move_only_function<TRet(TArgs...)>::move_only_function(TFunc&& func)
    : move_only_function(&move_only_funcs_for<TFunc>, construct(std::forward<TFunc>(func)))
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
    m_funcs = &move_only_funcs_for<TFunc>;
    m_data = construct_data(std::forward<TFunc>(func));
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
    if constexpr (is_function_pointer_convertible<TFunc>)
    {
      return reinterpret_cast<void*>(static_cast<TRet(*)(TArgs...)>(func));
    }
    else
    {
      return new TRawFunc<TFunc>(std::forward<TFunc>(func));
    }
  }

  template<typename TRet, typename... TArgs>
  TRet move_only_function<TRet(TArgs...)>::operator()(TArgs... args) const
  {
    assert(m_funcs);
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
      TBase::m_data = static_cast<const funcs*>(TBase::m_funcs)->copy(other.m_data);
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
    : TBase::move_only_function(&funcs_for<TFunc>, TBase::construct(std::forward<TFunc>(func)))
  {}

  template<typename TRet, typename... TArgs>
  template<typename TFunc, typename>
  inline auto function<TRet(TArgs...)>::operator=(TFunc&& func) -> function&
  {
    TBase::destruct();
    TBase::m_funcs = &funcs_for<TFunc>();
    TBase::m_data = construct_data(std::forward<TFunc>(func));
    return *this;
  }
}

