#include "include/llist.h"

void add_node(bin_t *bin, node_t* node) {
    node->next = NULL;
    node->prev = NULL;

    node_t *temp = bin->head;

    // 如果bin为空, 那么node就加入到bin中, 用head指向
    if (bin->head == NULL) {
        bin->head = node;
        return;
    }
    
    // current和previous用于在下面的迭代过程中保存结点.
    node_t *current = bin->head;
    node_t *previous = NULL;
    // 迭代直到我们到达链表末尾, 或是找到了一个合适的结点
    while (current != NULL && current->size <= node->size) {
        previous = current;
        current = current->next;
    }

    // 到达链表尾, 没有找到合适结点
    if (current == NULL) { 
        // 加入到链表尾部
        previous->next = node;
        node->prev = previous;
    }
    else {
        if (previous != NULL) {
            // 位置在链表中间, 前后都要连接
            node->next = current;
            previous->next = node;

            node->prev = previous;
            current->prev = node;
        }
        else { 
            // preivious为空, 而current不为空, 说明bin中head是唯一的结点. 
            // 将node添加到链表头部
            node->next = bin->head;
            bin->head->prev = node;
            bin->head = node;
        }
    }
}

void remove_node(bin_t * bin, node_t *node) {
    // 链表为空, 直接返回
    if (bin->head == NULL) return; 
    // 链表头就是所要移除的结点
    if (bin->head == node) { 
        bin->head = bin->head->next;
        return;
    }
    
    node_t *temp = bin->head->next;
    while (temp != NULL) {
        if (temp == node) { // 找到了要移除的结点
            if (temp->next == NULL) { // 如果是链表尾部最后一个结点
                temp->prev->next = NULL;
            }
            else { // 如果是在链表中间发现了这个结点
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            // 这里无需担心要移除head结点, 因为我们已经在函数开头已经做了检查
            return;
        }
        temp = temp->next;
    }
}

node_t *get_best_fit(bin_t *bin, size_t size) {
    if (bin->head == NULL) return NULL; // 空链表

    node_t *temp = bin->head;

    while (temp != NULL) {
        if (temp->size >= size) {
            return temp; // 找到不小于size的最合适结点!
        }
        temp = temp->next;
    }
    return NULL; // 没有合适的结点!
}

node_t *get_last_node(bin_t *bin) {
    node_t *temp = bin->head;
    
    while (temp->next != NULL) {
        temp = temp->next;
    }
    return temp;
}

