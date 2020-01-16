#pragma once

#include <algorithm>

namespace guddy {

  // Doubly linked list used for free lists
  struct list_t {
    struct list_t* prev;
    struct list_t* next;

    list_t() {
      this->init();
    }

    void init() {
      prev = this;
      next = this;
    }

    void push(list_t* entry) {
      list_t* prev = this->prev;
      entry->prev = prev;
      entry->next = this;
      prev->next = entry;
      this->prev = entry;
    }

    void remove(list_t* entry) {
      list_t* prev = entry->prev;
      list_t* next = entry->next;
      prev->next = next;
      next->prev = prev;
    }

    list_t* pop() {
      list_t* back = this->prev;
      if (back == this) return NULL;
      this->remove(back);
    }
  };

  struct allocator {
    allocator() {}
  };

}
