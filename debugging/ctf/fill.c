#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fill.h"

void
fillit(fill_t *fill)
{
	fill->a = 16;
	memset(fill->array, 'C', sizeof (fill->array));
}
