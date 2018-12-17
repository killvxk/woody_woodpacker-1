#include <unistd.h>
int main(void)
{
  write(1, "coucou\n", 7);
  return (0);
}
