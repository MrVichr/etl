///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2019 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

/******************************************************************************

Copyright (C) 2017 by Sergey A Kryukov: derived work
http://www.SAKryukov.org
http://www.codeproject.com/Members/SAKryukov

Based on original work by Sergey Ryazanov:
"The Impossibly Fast C++ Delegates", 18 Jul 2005
https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates

MIT license:
http://en.wikipedia.org/wiki/MIT_License

Original publication: https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed

******************************************************************************/

#ifndef ETL_DELEGATE_INCLUDED
#define ETL_DELEGATE_INCLUDED

#include "platform.h"
#include "error_handler.h"
#include "exception.h"
#include "type_traits.h"
#include "utility.h"
#include "optional.h"

#ifndef ETL_DELEGATE_SUPPORTS_CONSTEXPR
#define ETL_DELEGATE_SUPPORTS_CONSTEXPR 1
#endif

#if ETL_CPP11_NOT_SUPPORTED
  #if !defined(ETL_IN_UNIT_TEST)
    #error NOT SUPPORTED FOR C++03 OR BELOW
  #endif
#else
namespace etl
{
  //***************************************************************************
  /// The base class for delegate exceptions.
  //***************************************************************************
  class delegate_exception : public exception
  {
  public:

    delegate_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The exception thrown when the delegate is uninitialised.
  //***************************************************************************
  class delegate_uninitialised : public delegate_exception
  {
  public:

    delegate_uninitialised(string_type file_name_, numeric_type line_number_)
      : delegate_exception(ETL_ERROR_TEXT("delegate:uninitialised", ETL_DELEGATE_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //*************************************************************************
  /// Declaration.
  //*************************************************************************
  template <typename T, size_t ADDITIONAL_STORAGE_SIZE=0*sizeof(void *), size_t REQUIRED_ALIGNMENT=alignof(void *)> class delegate;

  //*************************************************************************
  /// Specialisation.
  //*************************************************************************
  template <typename TReturn, typename... TParams, size_t ADDITIONAL_STORAGE_SIZE, size_t REQUIRED_ALIGNMENT>
  class delegate<TReturn(TParams...), ADDITIONAL_STORAGE_SIZE, REQUIRED_ALIGNMENT> final
  {
    template <typename T, size_t x1, size_t x2>
    friend class delegate;

  public:

    //*************************************************************************
    /// Default constructor.
    //*************************************************************************
    ETL_CONSTEXPR14 delegate(): invocation(nullptr, nullptr)
    {
    }

    //*************************************************************************
    // Copy constructor.
    //*************************************************************************
    template <typename OTHER_RETURN, typename... OTHER_PARAMS, size_t STORAGEOF_OTHER, size_t ALIGNOF_OTHER>
    ETL_CONSTEXPR14 delegate(const delegate<OTHER_RETURN(OTHER_PARAMS...), STORAGEOF_OTHER, ALIGNOF_OTHER> &other)
      : invocation(memory_helper<sizeof(other.invocation.storage), sizeof(invocation.storage)-sizeof(other.invocation.storage)> (other.invocation.storage.opaque),
                   (const vmt_t *)other.invocation.vmt)
    {
      static_assert(std::is_same<TReturn(TParams...), OTHER_RETURN(OTHER_PARAMS...)>::value, "Must create from same type of delegate");
      static_assert(sizeof(other.invocation.storage)<=sizeof(invocation.storage));
      static_assert(ALIGNOF_OTHER<=REQUIRED_ALIGNMENT);
    }

    //*************************************************************************
    // Destructor
    //*************************************************************************
#if !(ETL_DELEGATE_SUPPORTS_CONSTEXPR)
    ~delegate()
    {
      if (invocation.vmt && invocation.vmt->destroy)
        (invocation.*(invocation.vmt->destroy))();
    }
#endif
    //*************************************************************************
    // Construct from lambda or functor.
    //*************************************************************************
    template <typename TLambda, typename = etl::enable_if_t<etl::is_class<TLambda>::value, void>>
    ETL_CONSTEXPR14 delegate(const TLambda& instance)
      : invocation((const vmt_t *)&vmt_wrapper<
                       &invocation_element::template lambda_call<TLambda>,
                       &invocation_element::template lambda_copy<TLambda>,
                       &invocation_element::template lambda_destroy<TLambda>>::vmt)
    {
      static_assert(sizeof(TLambda)<=sizeof(invocation_element::storage.opaque), "Insufficient storage in delegate");
      static_assert(alignof(TLambda)<=REQUIRED_ALIGNMENT, "Insufficient alignment of delegate");
      new (&invocation.storage.opaque) TLambda (instance);
    }

    //*************************************************************************
    /// Create from function (Compile time).
    //*************************************************************************
    template <TReturn(*Method)(TParams...)>
    ETL_CONSTEXPR14 static delegate create()
    {
      using function_vmt=vmt_wrapper<
        &invocation_element::template function_call<Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      return delegate<TReturn(TParams...), ADDITIONAL_STORAGE_SIZE, REQUIRED_ALIGNMENT>(nullptr, &function_vmt::vmt);
    }

    //*************************************************************************
    /// Create from Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = etl::enable_if_t<etl::is_class<TLambda>::value, void>>
    ETL_CONSTEXPR14 static delegate create(const TLambda& instance)
    {
      return delegate(instance);
    }

    //*************************************************************************
    /// Create from instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TParams...)>
    ETL_CONSTEXPR14 static delegate create(T& instance)
    {
      using method_vmt=vmt_wrapper<
        &invocation_element::template method_stub<T, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      return delegate((void*)(&instance), &method_vmt::vmt);
    }

    //*************************************************************************
    /// Create from instance method (Run time).
    /// Deleted for rvalue references.
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TParams...)>
    ETL_CONSTEXPR14 static delegate create(T&& instance) = delete; //should be fine now that we copy
    //*************************************************************************
    /// Create from const instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TParams...) const>
    ETL_CONSTEXPR14 static delegate create(const T& instance)
    {
      using const_method_vmt=vmt_wrapper<
        &invocation_element::template const_method_stub<T, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      return delegate((void*)(&instance), &const_method_vmt::vmt);
    }

    //*************************************************************************
    /// Disable create from rvalue instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::*Method)(TParams...) const>
    ETL_CONSTEXPR14 static delegate create(T&& instance) = delete; //same

    //*************************************************************************
    /// Create from instance method (Compile time).
    //*************************************************************************
    template <typename T, T& Instance, TReturn(T::*Method)(TParams...)>
    ETL_CONSTEXPR14 static delegate create()
    {
      using method_instance_vmt=vmt_wrapper<
        &invocation_element::template method_instance_stub<T, Instance, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      return delegate(ETL_NULLPTR, &method_instance_vmt::vmt);
    }

    //*************************************************************************
    /// Create from const instance method (Compile time).
    //*************************************************************************
    template <typename T, T const& Instance, TReturn(T::*Method)(TParams...) const>
    ETL_CONSTEXPR14 static delegate create()
    {
      using const_method_instance_vmt=vmt_wrapper<
        &invocation_element::template const_method_instance_stub<T, Instance, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      return delegate(ETL_NULLPTR, &const_method_instance_vmt::vmt);
    }

    //*************************************************************************
    /// Set from function (Compile time).
    //*************************************************************************
    template <TReturn(*Method)(TParams...)>
    ETL_CONSTEXPR14 void set()
    {
      using function_vmt=vmt_wrapper<
        &invocation_element::template function_call<Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      invocation.template assign<sizeof(void *), alignof (void *)>(ETL_NULLPTR, &function_vmt::vmt);
    }

    //*************************************************************************
    /// Set from Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = etl::enable_if_t<etl::is_class<TLambda>::value, void>>
    ETL_CONSTEXPR14 void set(const TLambda& instance)
    {
      using lambda_vmt=vmt_wrapper<
        &invocation_element::template lambda_call<TLambda>,
        &invocation_element::template lambda_copy<TLambda>,
        &invocation_element::template lambda_destroy<TLambda>
      >;
      invocation.template assign<sizeof(void *), alignof (void *)>((void*)(&instance), &lambda_vmt::vmt);
    }

    //*************************************************************************
    /// Set from instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TParams...)>
    ETL_CONSTEXPR14 void set(T& instance)
    {
      using method_vmt=vmt_wrapper<
        &invocation_element::template method_stub<T, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      invocation.template assign<sizeof(void *), alignof (void *)>((void*)(&instance), &method_vmt::vmt);
    }

    //*************************************************************************
    /// Set from const instance method (Run time).
    //*************************************************************************
    template <typename T, TReturn(T::* Method)(TParams...) const>
    ETL_CONSTEXPR14 void set(T& instance)
    {
      using const_method_vmt=vmt_wrapper<
        &invocation_element::template const_method_stub<T, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      invocation.template assign<sizeof(void *), alignof (void *)>((void*)(&instance), &const_method_vmt::vmt);
    }

    //*************************************************************************
    /// Set from instance method (Compile time).
    //*************************************************************************
    template <typename T, T& Instance, TReturn(T::* Method)(TParams...)>
    ETL_CONSTEXPR14 void set()
    {
      using method_instance_vmt=vmt_wrapper<
        &invocation_element::template method_instance_stub<T, Instance, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      invocation.template assign<sizeof(void *), alignof (void *)>(ETL_NULLPTR, &method_instance_vmt::vmt);
    }

    //*************************************************************************
    /// Set from const instance method (Compile time).
    //*************************************************************************
    template <typename T, T const& Instance, TReturn(T::* Method)(TParams...) const>
    ETL_CONSTEXPR14 void set()
    {
      using const_method_instance_vmt=vmt_wrapper<
        &invocation_element::template const_method_instance_stub<T, Instance, Method>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      invocation.template assign<sizeof(void *), alignof (void *)>(ETL_NULLPTR, &const_method_instance_vmt::vmt);
    }

#if !(defined(ETL_COMPILER_GCC) && (__GNUC__ < 8))
    //*************************************************************************
    /// Create from instance function operator (Compile time).
    /// At the time of writing, GCC appears to have trouble with this.
    //*************************************************************************
    template <typename T, T& Instance>
    ETL_CONSTEXPR14 static delegate create()
    {
      using operator_instance_vmt=vmt_wrapper<
        &invocation_element::template operator_instance_stub<T, Instance>,
        &invocation_element::pointer_copy,
        &invocation_element::pointer_destroy
      >;
      return delegate(ETL_NULLPTR, &operator_instance_vmt::vmt);
    }
#endif

    //*************************************************************************
    /// Execute the delegate.
    //*************************************************************************
    TReturn operator()(TParams... args) const
    {
      ETL_ASSERT(is_valid(), ETL_ERROR(delegate_uninitialised));

      return (invocation.*(invocation.vmt->call))(etl::forward<TParams>(args)...);
    }

    //*************************************************************************
    /// Execute the delegate if valid.
    /// 'void' return.
    //*************************************************************************
    template <typename TRet = TReturn>
    typename etl::enable_if_t<etl::is_same<TRet, void>::value, bool>
      call_if(TParams... args) const
    {
      if (is_valid())
      {
        (invocation.*(invocation.vmt->call))(etl::forward<TParams>(args)...);
        return true;
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    /// Execute the delegate if valid.
    /// Non 'void' return.
    //*************************************************************************
    template <typename TRet = TReturn>
    typename etl::enable_if_t<!etl::is_same<TRet, void>::value, etl::optional<TReturn>>
      call_if(TParams... args) const
    {
      etl::optional<TReturn> result;

      if (is_valid())
      {
        result = (invocation.*(invocation.vmt->call))(etl::forward<TParams>(args)...);
      }

      return result;
    }

    //*************************************************************************
    /// Execute the delegate if valid or call alternative.
    /// Run time alternative.
    //*************************************************************************
    template <typename TAlternative>
    TReturn call_or(TAlternative alternative, TParams... args) const
    {
      if (is_valid())
      {
        return (invocation.*(invocation.vmt->call))(etl::forward<TParams>(args)...);
      }
      else
      {
        return alternative(etl::forward<TParams>(args)...);
      }
    }

    //*************************************************************************
    /// Execute the delegate if valid or call alternative.
    /// Compile time alternative.
    //*************************************************************************
    template <TReturn(*Method)(TParams...)>
    TReturn call_or(TParams... args) const
    {
      if (is_valid())
      {
        return (invocation.*(invocation.vmt->call))(etl::forward<TParams>(args)...);
      }
      else
      {
        return Method(etl::forward<TParams>(args)...);
      }
    }

    //*************************************************************************
    /// operator=(delegate)
    //*************************************************************************
    template <typename OTHER_RETURN, typename... OTHER_PARAMS, size_t OTHER_STORAGE, size_t OTHER_ALIGNOF>
    delegate& operator =(const delegate<OTHER_RETURN(OTHER_PARAMS...), OTHER_STORAGE, OTHER_ALIGNOF> &rhs)
    {
      static_assert(std::is_same<TReturn(TParams...), OTHER_RETURN(OTHER_PARAMS...)>::value, "Must assign from same type of delegate");
      invocation.template assign<sizeof(rhs.invocation.storage), OTHER_ALIGNOF>
          (&rhs.invocation.storage, (const vmt_t *)rhs.invocation.vmt); //convert vmt, so we 1) hide some compiler errors 2) allow assignment to bigger storage
      return *this;
    }
    /*delegate& operator =(const delegate<TReturn(TParams...), ADDITIONAL_STORAGE_SIZE, REQUIRED_ALIGNMENT> &rhs)
    {
      invocation.template assign<sizeof(rhs.invocation.storage), REQUIRED_ALIGNMENT>
          (&rhs.invocation.storage, (const vmt_t *)rhs.invocation.vmt);
      return *this;
    }*/

    //*************************************************************************
    /// Create from Lambda or Functor.
    //*************************************************************************
    template <typename TLambda, typename = etl::enable_if_t<etl::is_class<TLambda>::value, void>>
    ETL_CONSTEXPR14 delegate& operator =(const TLambda& instance)
    {
      using lambda_vmt=vmt_wrapper<
        &invocation_element::template lambda_call<TLambda>,
        &invocation_element::template lambda_copy<TLambda>,
        &invocation_element::template lambda_destroy<TLambda>
      >;
      invocation.template assign<TLambda>(instance, &lambda_vmt::vmt);
      return *this;
    }

    //*************************************************************************
    /// Checks equality.
    //*************************************************************************
    template <class T, size_t OTHER_SIZE, size_t OTHER_ALIGN> //if we force correct T, we could get converted to bool and then compared
    ETL_CONSTEXPR14 bool operator == (const delegate<T, OTHER_SIZE, OTHER_ALIGN>& rhs) const
    {
      return ((const vmt_t *)rhs.invocation.vmt == invocation.vmt) &&
             (rhs.invocation.storage.object == invocation.storage.object);
    }

    //*************************************************************************
    /// Returns <b>true</b> if the delegate is valid.
    //*************************************************************************
    template <class T, size_t OTHER_SIZE, size_t OTHER_ALIGN>
    ETL_CONSTEXPR14 bool operator != (const delegate<T, OTHER_SIZE, OTHER_ALIGN>& rhs) const
    {
      static_assert(std::is_same<TReturn(TParams...), T>::value, "Must compare same type of delegate");
      return ((const vmt_t *)rhs.invocation.vmt != invocation.vmt) ||
             (rhs.invocation.storage.object != invocation.storage.object);
    }

    //*************************************************************************
    /// Returns <b>true</b> if the delegate is valid.
    //*************************************************************************
    ETL_CONSTEXPR14 bool is_valid() const
    {
      return invocation.vmt != ETL_NULLPTR; //never happens: && invocation.vmt->call != ETL_NULLPTR
    }

    //*************************************************************************
    /// Returns <b>true</b> if the delegate is valid.
    //*************************************************************************
    ETL_CONSTEXPR14 operator bool() const
    {
      return is_valid();
    }

  private:

    struct invocation_element;

    using vmt_call_t = TReturn(invocation_element::*)(TParams...) const;
    using vmt_copy_t = void (invocation_element::*)(const void *source);
    using vmt_destroy_t = void (invocation_element::*)();
    using vmt_t = struct
    {
      vmt_call_t call;
      vmt_copy_t copy;
      vmt_destroy_t destroy;
    };

    template <vmt_call_t pcall, vmt_copy_t pcopy, vmt_destroy_t pdestroy>
    struct vmt_wrapper //work around "static variable not permitted in a constexpr function"
    {
      static constexpr vmt_t vmt={
        .call = pcall,
        .copy = pcopy,
        .destroy = pdestroy
      };
    };

    //helper to copy N bytes, fill the rest with 0x00
    template <size_t data_size, size_t padding_size> struct memory_helper;
    template <size_t data_size, size_t padding_size>
    struct memory_helper
    {
      using data_helper=struct {
        uint8_t data[data_size];
      };
      constexpr memory_helper(): data(), padding {} {}
      constexpr memory_helper(const uint8_t *source): data(*(const data_helper *)source), padding {} {}
      data_helper data;
      uint8_t padding[padding_size];
    };


    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      ETL_CONSTEXPR14 invocation_element(const vmt_t *vmt): storage(), vmt(vmt)
      {
      }

      //template <size_t object_size, size_t object_align>
      ETL_CONSTEXPR14 invocation_element(const uint8_t (&source)[sizeof(void *)+ADDITIONAL_STORAGE_SIZE], const vmt_t *vmt):
        storage(source), vmt(vmt)
      {
      }
      template <size_t x1, size_t x2>
      ETL_CONSTEXPR14 invocation_element(const memory_helper<x1, x2> &source, const vmt_t *vmt):
        storage(*(const memory_helper<x1+x2, 0> *)&source), vmt(vmt)
      {
      }
      //***********************************************************************
      ETL_CONSTEXPR14 invocation_element(const void *pointer, const vmt_t *vmt)
        : storage(pointer), vmt(vmt)
      {
      }

      //*************************************************************************
      /// Assign from an object and stub.
      //*************************************************************************
      template <size_t object_size, size_t object_align>
      ETL_CONSTEXPR14 void assign(const void *source, const vmt_t *vmt)
      {
        static_assert(object_size<=sizeof(storage.opaque), "Insufficient storage in delegate");
        static_assert(object_align<=REQUIRED_ALIGNMENT, "Insufficient alignment of delegate");
        if (this->vmt && this->vmt->destroy)
          (this->*(this->vmt->destroy))();
        new (storage.opaque) memory_helper<object_size, sizeof(storage.opaque)-object_size>
            ((const uint8_t *)&source);
        if (vmt)
        {
          assert(vmt->copy);
          (this->*(vmt->copy))(source);
        }
        this->vmt = vmt;
      }

      template <class TLambda>
      ETL_CONSTEXPR14 void assign(const TLambda &source, const vmt_t *vmt)
      {
        static_assert(sizeof(TLambda)<=sizeof(storage.opaque), "Insufficient storage in delegate");
        static_assert(alignof(TLambda)<=REQUIRED_ALIGNMENT, "Insufficient alignment of delegate");
        if (this->vmt && this->vmt->destroy)
          (this->*(this->vmt->destroy))();
        //if (sizeof(storage.opaque)>sizeof(TLambda)) //some memset fail with size==0
          //memset(&storage.opaque[sizeof(TLambda)], 0xff, sizeof(storage.opaque)-sizeof(TLambda));
        new (storage.opaque) memory_helper<sizeof(TLambda), sizeof(storage.opaque)-sizeof(TLambda)>
            ((const uint8_t *)&source);
        if (vmt)
        {
          assert(vmt->copy);
          (this->*(vmt->copy))(&source);
        }
        this->vmt = vmt;
      }

      //***********************************************************************
      /*ETL_CONSTEXPR14 bool operator ==(const invocation_element& rhs) const
      {
        return (rhs.vmt == vmt) && (rhs.storage.object == storage.object);
      }*/

      //***********************************************************************
      /*ETL_CONSTEXPR14 bool operator !=(const invocation_element& rhs) const
      {
        return (rhs.vmt != vmt) || (rhs.storage.object != storage.object);
      }*/

      //*************************************************************************
      /// Stub call for a member function. Run time instance.
      //*************************************************************************
      template <typename T, TReturn(T::*Method)(TParams...)>
      ETL_CONSTEXPR14 TReturn method_stub(TParams... params) const
      {
        T* p = *(T **)(&storage.object);
        return (p->*Method)(etl::forward<TParams>(params)...);
      }

      //*************************************************************************
      /// Stub call for a const member function. Run time instance.
      //*************************************************************************
      template <typename T, TReturn(T::*Method)(TParams...) const>
      ETL_CONSTEXPR14 TReturn const_method_stub(TParams... params) const
      {
        T* const p = *(T **)(&storage.object);
        return (p->*Method)(etl::forward<TParams>(params)...);
      }

      //*************************************************************************
      /// Stub call for a member function. Compile time instance.
      //*************************************************************************
      template <typename T, T& Instance, TReturn(T::*Method)(TParams...)>
      ETL_CONSTEXPR14 TReturn method_instance_stub(TParams... params) const
      {
        return (Instance.*Method)(etl::forward<TParams>(params)...);
      }

      //*************************************************************************
      /// Stub call for a const member function. Compile time instance.
      //*************************************************************************
      template <typename T, const T& Instance, TReturn(T::*Method)(TParams...) const>
      ETL_CONSTEXPR14 TReturn const_method_instance_stub(TParams... params) const
      {
        return (Instance.*Method)(etl::forward<TParams>(params)...);
      }

  #if !(defined(ETL_COMPILER_GCC) && (__GNUC__ < 8))
      //*************************************************************************
      /// Stub call for a function operator. Compile time instance.
      //*************************************************************************
      template <typename T, T& Instance>
      ETL_CONSTEXPR14 TReturn operator_instance_stub(TParams... params) const
      {
        return Instance.operator()(etl::forward<TParams>(params)...);
      }
  #endif

      //*************************************************************************
      /// Stub call for a free function.
      //*************************************************************************
      template <TReturn(*Method)(TParams...)>
      ETL_CONSTEXPR14 TReturn function_call(TParams... params) const
      {
        return (Method)(etl::forward<TParams>(params)...);
      }

      ETL_CONSTEXPR14 void pointer_copy(const void *source)
      {
        new (&storage.object) (const void *)(source);
      }

      ETL_CONSTEXPR14 void pointer_destroy()
      {
        storage.object=nullptr;
      }
      //*************************************************************************
      /// Stub call for a lambda or functor function.
      //*************************************************************************
      template <typename TLambda>
      ETL_CONSTEXPR14 TReturn lambda_call(TParams... arg) const
      {
        TLambda *p = (TLambda *)(&storage.object);
        return (p->operator())(etl::forward<TParams>(arg)...);
      }

      template <typename TLambda>
      ETL_CONSTEXPR14 void lambda_copy(const void *source)
      {
        new (&storage.opaque) TLambda (*(const TLambda *)source);
      }

      template <typename TLambda>
      ETL_CONSTEXPR14 void lambda_destroy()
      {
        TLambda *p = (TLambda *)(&storage.object);
        p->~TLambda();
      }

      //***********************************************************************
      alignas(REQUIRED_ALIGNMENT) union Storage
      {
        constexpr Storage(const void *pointer): object(pointer) {}
        constexpr Storage(const uint8_t (&source)[sizeof(void *)+ADDITIONAL_STORAGE_SIZE]): opaque(source) {}
        constexpr Storage(const memory_helper<sizeof(void *)+ADDITIONAL_STORAGE_SIZE, 0> &source): _forinitN(source) {}
        constexpr Storage(): _forinit0() {}
        const void *object;
        uint8_t opaque[sizeof(object)+ADDITIONAL_STORAGE_SIZE];
        memory_helper<sizeof(void *)+ADDITIONAL_STORAGE_SIZE, 0> _forinitN;
        memory_helper<0, sizeof(void *)+ADDITIONAL_STORAGE_SIZE> _forinit0;
      } storage;
      const vmt_t *vmt = ETL_NULLPTR;
    };

    //*************************************************************************
    /// Constructs a delegate from an object and stub.
    //*************************************************************************
    ETL_CONSTEXPR14 delegate(const uint8_t (&object)[sizeof(void *)+ADDITIONAL_STORAGE_SIZE], const vmt_t *vmt)
      : invocation(object, vmt)
    {
    }

    //*************************************************************************
    /// Constructs a delegate from a stub.
    //*************************************************************************
    ETL_CONSTEXPR14 delegate(const void *pointer, const vmt_t *vmt)
      : invocation(pointer, vmt)
    {
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };
}

#endif

#endif
