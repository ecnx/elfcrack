/* ------------------------------------------------------------------
 * ELF SysV Hash Crack
 * ------------------------------------------------------------------ */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/**
 * Calculate ELF symbol hash
 */
static unsigned long elf_hash ( const char *name )
{
    unsigned long h = 0, g;

    while ( *name )
    {
        h = ( h << 4 ) + *name++;
        if ( ( g = h & 0xf0000000 ) )
        {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h;
}

/**
 * Filter symbol name character
 */
static int symbol_name_char_blacklisted ( char c )
{
    return !isalnum ( c ) && c != '_';
}

/**
 * Process potential result
 */
static void test_result (const char* symbol_name, unsigned long orig_hash, const char* workbuf, int* found)
{
    unsigned long calc_hash;
    
    calc_hash= elf_hash ( workbuf );
    
    if ( calc_hash == orig_hash && strcmp ( workbuf, symbol_name ))
    {
        puts(workbuf);
        *found += 1;
    }
}

/**
 * Generate similar symbol names by exact hash
 */
static void gen_case(const char* symbol_name, unsigned long orig_hash, char* workbuf, size_t length, size_t depth, int* found, int limit)
{
    int i;
    int var_found = 0;

    if (depth >= length)
    {
        return;
    }
    
    for (i = 0; i < 256; i++, workbuf[depth]++)
    {
        if ( !symbol_name_char_blacklisted ( workbuf[depth] ) )
        {
            test_result (symbol_name, orig_hash, workbuf, &var_found);
            
            if (var_found >= limit)
            {
                break;
            }
            
            gen_case(symbol_name, orig_hash, workbuf, length, depth + 1, &var_found, limit);
            
            if (var_found >= limit)
            {
                break;
            }
        }
    }
    
    *found = var_found;
}

/**
 * Show program usage message
 */
static void show_usage(void)
{
    fprintf ( stderr, "\n  usage: elfcrack limit symbol-name\n\n" );
}

/**
 * Program startup
 */
int main ( int argc, char *argv[] )
{
    int found = 0;
    int limit;
    size_t length;
    const char *symbol_name;
    unsigned long orig_hash;
    char workbuf[256];

    /* Show program banner */
    /* printf ( "ELF SysV Hash Crack - ver. 1.0.15\n" ); */

    /* Check arguments count */
    if ( argc != 3 )
    {
        show_usage();
        return 1;
    }

    /* Parse match limit */
    if (sscanf(argv[1], "%i", &limit) < 0)
    {
        show_usage();
        return 1;        
    }
    
    /* Assign command line arguments */
    symbol_name = argv[2];

    /* Calculate symbol name length */
    length = strlen ( symbol_name );

    /* Verify symbol name length */
    if ( length < 1 || length >= sizeof ( workbuf ) )
    {
        fprintf ( stderr, "symbol name is invalid!\n" );
        return 1;
    }

    /* Calculate original hash */
    orig_hash = elf_hash ( symbol_name );

    /* Copy symbol name to workbuf */
    memcpy ( workbuf, symbol_name, length + 1 );

    /* Find similar symbol names by exact hash */
    gen_case(symbol_name, orig_hash, workbuf, length, 0, &found, limit);
    
    return 0;
}
