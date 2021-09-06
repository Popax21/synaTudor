#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libfprint/fprint.h>

int main (int argc, char **argv)
{
  FpContext *ctx;

  ctx = fp_context_new ();
  g_object_unref (ctx);

  return 0;
}
