#include <linux/module.h>
#include <linux/kallsyms.h>
//#include "ptr.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("k4y0z");

unsigned int* _selinux_enforcing = NULL;

unsigned int* find_selinux_enforcing(void){
  char *sym_sel_read_enforce = (char *)kallsyms_lookup_name("sel_read_enforce");
  int i=0;
  uint64_t selinux_temp = 0;
  const char asm_sub[] = { 0xf3, 0x53, 0x01, 0xa9 };
  printk("sym_rel_read_enforce_addr: %p\n", sym_sel_read_enforce);

  //selinux_temp = 0xffffffc001919bcc;

  for (i =0; i < 128; i++){
    //printk("0x%X ", sym_sel_read_enforce[i]);
    if(memcmp(&sym_sel_read_enforce[i], asm_sub, 4) == 0){
      selinux_temp = 0;
      selinux_temp = (*(unsigned int*)&sym_sel_read_enforce[i+12] & 0xFFFFE0) << 2; 
      selinux_temp |= (*(unsigned int*)&sym_sel_read_enforce[i+12] & 0x60000000); 
      printk("selinux_temp: %p\n", (unsigned int *)selinux_temp);
      // Sign-extend the 21-bit immediate.
      if (selinux_temp & (1 << (21 - 1))) 
        selinux_temp |= ~((1LL << 21) - 1);
      selinux_temp = (selinux_temp & 0xFFFFFF) | ((selinux_temp >> 24) & 0xFF);
      selinux_temp <<= 7;
      selinux_temp += ((uint64_t)sym_sel_read_enforce & 0XFFFFFFFFFFFFF000);
      selinux_temp |= ((*(unsigned int*)&sym_sel_read_enforce[i+28] & 0x000FFF00)) >> 8; 
      return (unsigned int*)selinux_temp;
    }
  }
  return 0;
}

static int __init selinux_module_init(void){
  _selinux_enforcing = find_selinux_enforcing();
  if(_selinux_enforcing){
    printk("[k4y0z]: found selinux_enforcing at %p\n", _selinux_enforcing);
    printk("[k4y0z]: set selinux to permissive\n");
    *_selinux_enforcing = 0;
  }
  else{
    printk("[k4y0z]: didn't find selinux_enforcing\n");
  }
  
  return 0;
}

static void __exit selinux_module_exit(void){
  printk("[k4y0z]: Byebye!\n");
}
module_init(selinux_module_init)
module_exit(selinux_module_exit)
