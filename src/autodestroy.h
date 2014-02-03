/*
 * This template class is a very simple class which allows registration of items
 * needing destruction.
 *
 * Very simple: a set of items to destroy, and a map to keep track of how many
 * destruction references there are (allowing add and remove to be called
 * more than once).
 *
 * It is split into two parts (the AutoDestroy class and Destroyer) so that
 * the actual destruction call can be overridden (by specializing Destroyer).
 */
 
 
#include <set>
#include <map>
 
namespace Shell
{
  template <typename T> class Destroyer
  {
  public:
    Destroyer(T *what)
    {
      this->ptr = what;
    }
 
    void destroy()
    {
      delete ptr;
    }
 
  protected:
    T *ptr;
  };
 
  template <typename T> class AutoDestroy
  {
  public:
    static void Add(T *what)
    {
      Instance()->add(what);
    }
 
    static void Remove(T *what)
    {
      Instance()->remove(what);
    }
 
    virtual ~AutoDestroy()
    {
      typename std::set<T *>::iterator i;
      for (i = autoDestroy.begin(); i != autoDestroy.end(); i++)
      {
        Destroyer<T> destroyer(*i);
        destroyer.destroy();
      }
    }
 
    static AutoDestroy *Instance()
    {
      static AutoDestroy instance;
      return &instance;
    }
  protected:
    std::map<T *, int> references;
    std::set<T *> autoDestroy;
 
    void add(T *what)
    {
      //if it doesn't exist yet, add it.
      if (autoDestroy.count(what) < 1)
      {
        //insert into auto destroy list
        autoDestroy.insert(what);
 
        //set reference to 1
        references[what] = 1;
 
        //return because references is already 1
        return;
      }
 
      //increment reference
      references[what]++;
    }
 
    void remove(T *what)
    {
      //if it doesn't exist, return
      if (autoDestroy.count(what) < 1)
        return;
 
      //decrement references, and if there aren't any, remove from list.
      if (--references[what] <= 0)
      {
        autoDestroy.erase(what);
        references.erase(what);
      }
    }
  };
 
  /* Helper functions to make things dead simple */
  template<typename T> void autoDestroy(T *what)
  {
    AutoDestroy<T>::Add(what);
  }
 
  template<typename T> void cancelAutoDestroy(T *what)
  {
    AutoDestroy<T>::Remove(what);
  }
}

