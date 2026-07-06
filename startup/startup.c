#include <stdint.h>


extern uint32_t _estack;

extern uint32_t _etext;

extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sidata;

extern uint32_t _sbss;
extern uint32_t _ebss;


int main(void);
void reset_handler(void);

uint32_t vectors[] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&_estack,
    (uint32_t)&reset_handler,
};

void reset_handler(void)
{
    uint32_t *src = &_sidata, *dest = &_sdata;
    while(dest < &_edata)
    {
        *dest++ = *src++;
    }

    src = &_sbss;
    while(src < &_ebss)
    {
        *src++ = 0;
    }

    main();

}
