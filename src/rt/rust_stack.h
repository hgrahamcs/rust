#ifndef RUST_STACK_H
#define RUST_STACK_H

struct stk_seg {
    stk_seg *prev;
    stk_seg *next;
    uintptr_t end;
    unsigned int valgrind_id;
#ifndef _LP64
    uint32_t pad;
#endif

    uint8_t data[];
};

// A value that goes at the end of the stack and must not be touched
const uint8_t stack_canary[] = {0xAB, 0xCD, 0xAB, 0xCD,
                                0xAB, 0xCD, 0xAB, 0xCD,
                                0xAB, 0xCD, 0xAB, 0xCD,
                                0xAB, 0xCD, 0xAB, 0xCD};

void
add_stack_canary(stk_seg *stk);

template <class T>
stk_seg *
create_stack(T allocer, size_t sz) {
  size_t total_sz = sizeof(stk_seg) + sz + sizeof(stack_canary);
  stk_seg *stk = (stk_seg *)allocer->malloc(total_sz, "stack");
  memset(stk, 0, sizeof(stk_seg));
  add_stack_canary(stk);
  stk->end = (uintptr_t) &stk->data[sz];
  return stk;
}

template <class T>
void
destroy_stack(T allocer, stk_seg *stk) {
  allocer->free(stk);
}

void
config_valgrind_stack(stk_seg *stk);

void
unconfig_valgrind_stack(stk_seg *stk);

void
check_stack_canary(stk_seg *stk);

#endif /* RUST_STACK_H */
