# SHMALL - Simple Heap Memory ALLocator
------------
这是作者[@CCareaga](https://github.com/CCareaga)为OS爱好者而编写的一个简单堆分配器. CCareaga编写的这个堆分配器力图尽可能易于理解. 希望能帮到那些OS开发的初学者以及那些对malloc和free的简单函数实现感兴趣的人.

---
![SHAMLL](SHMALL.png "SHMALL")
---

### Compiling
------------
代码中包含两个头文件, 各自用于定义堆和链表.

编译命令:
``` 
$ gcc main.c llist.c heap.c -o heap_test 
$ ./heap_test
```

这会运行分配器的一个demo并输出一些信息.

### Explanation
------------

##### Initialization:

需要提供一块内存用以初始化堆空间. 在这里是用的`malloc`进行分配. `heap_t`结构中的bins也同样需要分配内存

调用函数`init_heap`必须要提供一个空白堆结构的地址. 函数`init_heap`会创建一个chunk, chunk包含有一个header(`node_t`结构)和一个footer(`footer_t`结构). 函数需要使用常量`HEAP_INIT_SIZE`来确定chunk的大小, 并将其添加到`start`参数中以便确定heap的终结位置.

##### Metadata and Design:

每个chunk的内存都包含一个位于开头的node结构和一个位于结尾的footer结构. 无论chunk被释放与否, node结构都包含有chunk的size以及2个用于双向链表中的指针(next和prev). footer结构只包含一个指向header的指针(当要释放相邻chunk时会用到这个指针). 在堆末尾的chunk被称为`wilderness chunk`. 它是堆中最大的chunk并且它的最大最小值都有在heap.h中声明. 合并或扩展chunk时就可以通过伸缩wilderness chunk来实现. 被释放的chunk存储在`bin`里, 每个bin实际上只是一个由大小相近的node组成的双向链表. 堆结构包含一定数量的bin, 数量在heap.h里的`BIN_COUNT`有定义. 要确定哪个bin放哪种chunk, 使用函数`get_bin_index`, 将chunk的size作为索引进行确定. 

这种一致的binning函数可以确保chunk可以以预定的方式来访问和存储. chunks按顺序插入到bin中, 所以chunk的插入操作并不是O(1), 但也变得更加方便查找最合适的chunk. 注意, 你可以自己定义binning函数, 尽管默认的binning函数的表现已经足够优秀. 确定一个可以帮助快速检索chunk的更复杂binning函数也许是十分值得的. 

##### Allocation:

函数`heap_alloc`取刚刚分配的heap结构地址以及一个size作为参数. 使用函数`get_bin_index`来确定对应size的chunk的位置, 当然也许并没有那个size的chunk存在. 如果没有在对应的bin中找到合适的chunk, 那么就会检查下一个bin, 直到找到一个合适的chunk, 或是在找的过程中到达了最后一个bin(也没有找到), 这种情况下会从wilderness堆块中取一块内存出来创建chunk. 如果找到的chunk过大, 那么就会将该chunk分割一小块回收进bin里. 在判定一个chunk是否需要分割的时候, 会减去chunk中不需要使用的元数据(overhead)的大小, 根据剩下的size来确定. 如果chunk分割后, chunk左边的部分内存大于或等于`MIN_ALLOC_SZ`, 那我们就应该继续分割chunk并将剩余部分放到合适的bin中. 一旦我们准备好返回我们找到的chunk, 我们返回chunk的`next`地址. 这是因为分配chunk时我们并没有用到`next`和`prev`, 因此chunk的使用者可以将数据写进`next`域, 而这不会对堆的内部工作产生丝毫影响.

##### Freeing: 

函数`heap_free`获取`heap_alloc`返回的指针. 通过减去合适的偏移量以获取node结构的正确地址. 函数`heap_free`并不是简单地将chunk放置在对应的bin里就行了, 还会检查chunk周边的其他chunk. 如果周边的chunk处于空闲状态那么我们就可以将这些chunk合并成一个更大的chunk. 为了合并这些chunk, 我们需要使用footer来获取前一个chunk和后一个chunk的node结构. 比方说, 我们有一个叫`to_free`的chunk. 我们减去`sizeof(footer_t)`可以获取前一个chunk的footer, 这个footer包含一个指向前一个chunk头部的指针. 而获取后一个chunk , 我们相反加上`sizeof(footer_t)`即可. 当合并完毕, 我们会重新计算合并后的chunk的大小并放置进bin里


### Sources 
------------
* [Doug Lea's Memory Allocator](http://g.oswego.edu/dl/html/malloc.html)

