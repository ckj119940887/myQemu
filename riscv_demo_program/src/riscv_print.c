#include "platform.h"

static char out_buf[1000];

static int _vsnprintf(char *out, size_t n, const char* s, va_list vl)
{
  int format = 0;
  int longarg = 0;
  size_t pos = 0;

  for(; *s; s++)
  {
    if(format) {
      switch(*s) {
        case 'l': 
        {
          longarg = 1;
          break;
        }//end for 'l'
        case 'p':
        {
          longarg = 1;
          if(out && pos < n)
          {
            out[pos] = '0';
            pos ++;
          }
          if(out && pos < n)
          {
            out[pos] = 'x';
            pos ++;
          }
        }//end for 'p'
        case 'x':
        {
          if(out && pos < n)
            out[pos] = '0';
          pos ++;

          if(out && pos < n)
            out[pos] = 'x';
          pos ++;

          long num = longarg ? va_arg(vl, long): va_arg(vl, int);
          int hexdigits = 2*(longarg ? sizeof(long): sizeof(int)) - 1;
          for(int i = hexdigits; i >= 0; i--)
          {
            int digit = (num >> (4 * i)) & 0xF;
            if(out && pos < n)
            {
              out[pos] = digit < 10 ? '0' + digit : 'A' + (digit - 10); 
            }
            pos ++;
          }
          longarg = 0;
          format = 0;
          break;
        }//end for 'x'
        case 'd': 
        {
          long num = longarg ? va_arg(vl, long): va_arg(vl, int);
          if(num < 0)
          {
            num = -num;
            if(out && pos < n)
            {
              out[pos] = '-';
            }
            pos ++;
          }

          long digits = 1;
          for(long nn = num; nn /= 10; digits++);
          for(long i = digits - 1; i >= 0; i--)
          {
            if(out && pos + i < n)
            {
              out[pos + i] = '0' + num % 10;
              num /= 10;
            }
          }
          pos += digits;
          longarg = 0;
          format = 0;
          break;
        }//end for 'd'
        case 's': 
        {
          const char *s2 = va_arg(vl, const char *);
          while(*s2) {
            if(out && pos < n)
            {
              out[pos] = *s2;
            }
            s2 ++;
            pos ++;
          }
          longarg = 0;
          format = 0;
          break;
        }//end for 's'
        case 'c':
        {
          const char *s2 = va_arg(vl, const char*);
          if(out && pos < n)
            out[pos] = *s2;
          pos ++;
          longarg = 0;
          format = 0;
          break;
        }//end for 'c'
        default:
          break;
      }
    }
    else if(*s == '%')
    {
      format = 1;
    }
    else 
    {
      if(out && pos < n)
        out[pos] = *s;
      pos ++;
    }
  }
  if(out && pos < n)
    out[pos] = 0;
  else if(out && n)
    out[n - 1] = 0;
  
  return pos;
}

static int _vprintf(const char *s, va_list vl)
{
  int res = _vsnprintf(NULL, 0, s, vl);
  if(res + 1 > sizeof(out_buf)) {
    uart_puts("error: output string overflow\n");
    while(1) {}
  }
  _vsnprintf(out_buf, res + 1, s, vl);
  uart_puts(out_buf);
  return res;
}

int riscv_print(const char *s, ...)
{
  int res = 0;
  va_list vl;
  va_start(vl, s);
  res = _vprintf(s, vl); 
  va_end(vl);
  return res;
}

void panic(char *s)
{
  riscv_print("panic:");
  riscv_print(s);
  riscv_print("\n");
  while(1) {};
}
