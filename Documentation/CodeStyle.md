# Code styles and approach

Project actively uses GLib and there are possibilty to declare "classes". But for it should be declared only in cases when:

* This is fundamental data type;
* There is a necesity to expand it functionality in future by inheritance.

All struct fiels that represents private fields should be written like this "_name".
All struct fiels that represents protected fields should be written like this "name".
Incupsulation should be respected and struct fields should be hidden from using in any place accept file with "class" definition. All data should be taken through get methods.

Structures should be named with Camel casing (ClassType). 

All methods should be named with snake case (word_word_word ()). Return value and access modifier should be placed on the one line, name and argument list on another. If there is more than two method parameters they should be aligned in a colunm. Here is a method declaration example:

```C
static ReturnType*
namespace_class_type_method_name(ClassType* this,
                                 int par1,
                                 char* par2,
                                 void* data);
```

Becouse of debug complexity any macro should not be defined. Only macros provided with libraries should be used. It should be as few of them as possible.

When there is a choise a crossplatform solution should be chosen.