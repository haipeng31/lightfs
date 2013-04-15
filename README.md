This is a simple(can not be more simpler) distributed file system,
just begins.

    2013/4/15
    派生类的智能指针可以隐式转化到基类的智能指针，如果通过下面的方法：
    shared_ptr<Base> baseNode(new Base());
    shared_ptr<Derived>(baseNode.get());
    一般都会悲剧(多次free).
    派生类的智能指针可以隐式转化到基类的智能指针是正常的，如果需要基类的智能指针转化到派生类的智能指针，一般都是设计上出现了问题
    需要看点智能指针的东西:)
