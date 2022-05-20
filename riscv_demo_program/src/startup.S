.section .text

.equ STACKSIZE, 4096 

.global _start
.extern start_kernel

_start:
  csrr t0, mhartid
  mv tp, t0
  bnez t0, deadloop

  #initialize bss data
  la a0, _bss_start
  la a1, _bss_end
  bgeu a0, a1, 2f
1:
  sw zero, (a0)
  addi a0, a0, 4
  bltu a0, a1, 1b
2:
  
  # setup stack
  slli t0, t0, 10
  la sp, stack_end
  add sp, sp, t0

  # main function
  j start_kernel

deadloop:
  wfi
  j deadloop

stack_start:
  .rept STACKSIZE 
  .word 0
  .endr
stack_end:

.end
