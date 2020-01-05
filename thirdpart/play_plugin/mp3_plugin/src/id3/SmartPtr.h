#ifndef APE_SMARTPTR_H
#define APE_SMARTPTR_H

#include <string.h>

namespace em_mp3_tag
{

// disable the operator -> on UDT warning
#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4284)
#endif

/*************************************************************************************************
CSmartPtr - a simple smart pointer class that can automatically initialize and free memory
    note: (doesn't do garbage collection / reference counting because of the many pitfalls)
*************************************************************************************************/
template <class TYPE> class CSmartPtr
{
public:
    TYPE* mp_object;
    bool mb_array;
    bool mb_delete;

    CSmartPtr()
    {
        mb_delete = true;
        mp_object = NULL;
    }
    CSmartPtr(TYPE* ap_object, bool ab_array = false, bool ab_delete = true)
    {
        mb_delete = true;
        mp_object = NULL;
        Assign(ap_object, ab_array, ab_delete);
    }

    ~CSmartPtr()
    {
        Delete();
    }

    void Assign(TYPE* ap_object, bool ab_array = false, bool ab_delete = true)
    {
        Delete();

        mb_delete = ab_delete;
        mb_array = ab_array;
        mp_object = ap_object;
    }

    void Delete()
    {
        if (mb_delete && mp_object != NULL)
        {
            if (mb_array)
                delete[] mp_object;
            else
                delete mp_object;
            mp_object = NULL;
        }
    }

    void SetDelete(bool ab_delete)
    {
        mb_delete = ab_delete;
    }

    __inline TYPE* get_ptr() const
    {
        return mp_object;
    }

    __inline operator TYPE* () const
    {
        return mp_object;
    }

    __inline TYPE* operator ->() const
    {
        return mp_object;
    }

    // declare assignment, but don't implement (compiler error if we try to use)
    // that way we can't carelessly mix smart pointers and regular pointers
    __inline void * operator =(void *) const;
};

#ifdef _MSC_VER
    #pragma warning(pop)
#endif _MSC_VER

}		//end namespace em_mp3_tag

#endif // #ifndef APE_SMARTPTR_H
