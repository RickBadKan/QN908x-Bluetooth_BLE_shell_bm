// =============================================================================
/**
\file    string_tools.c
\brief   Ferramentas de propósito geral para manipulação de strings.

\details
   Biblioteca de propósito geral para manipulação de string e conversão de
   valores numéricos para strings.

   Para habilitar a rotina de depuração, veja a constante:
     LOGICALIS_DEBUG_STRING_TOOLS

\b@{Histórico de Alterações:@}
    - 2019.03.12 -- removido o prefixo "isd" e surgiu a "string_tools.h/.c".
                 (v1.0.2)
    - 2018.12.09 -- Adicionados comentários
                 -- Criada isd_stringNCat()
                 (v1.0.1)
    - 2018.12.04 -- Primeira versão (v1.0.0)


\author
  Wagner A. P. Coimbra
*/
// =============================================================================
#include "string_tools.h"
#include <stdio.h> // REMOVE AFTER


//==============================================================================
//
// API
//
//==============================================================================

/**
 * @brief   Converte um caractere maiúsculo para minúsculo.
 *
 * @param   c     Caractere a ser convertido para minúscula.
 *
 * @return  o caractere em letra minúscula ou o caractere original.
 */
char lowercase(char c)
{
    // Trata o caso de ser um caractere normal
    if (c >= 'A' && c <= 'Z')
        return (char) (c + 32);

    // É outro caractere: não faz alterações
    return c;
}

/**
 * @brief   Converte a string informada para seu equivalente com letras minúsculas.
 *          O tamanho da string de entrada está limitado a um máximo de
 *             LOGICALIS_MEDIUM_STRING_MAX_LENGTH
 *          caracteres, incluindo o terminador nulo.
 *
 * @param   s     String a ser convertida para minúscula.
 *
 * @return  a string convertida (sem alterar a string de entrada).
 */
char *strToLower(char *s)
{
    static char buf[LOGICALIS_MEDIUM_STRING_MAX_LENGTH];
    int32_t i = 0;

    // Zera o buffer interno
    for (i = 0; i < LOGICALIS_MEDIUM_STRING_MAX_LENGTH; i++)
    {
        buf[i] = '\0';
    }

    // Adiciona no buffer interno a string de entrada, convertida para minúsculas
    i = 0;
    while (*s && i < LOGICALIS_MEDIUM_STRING_MAX_LENGTH)
    {
        buf[i++] = lowercase(*s++);
    }

    // Garante haja um terminado nulo
    buf[LOGICALIS_MEDIUM_STRING_MAX_LENGTH-1] = '\0';

    // Retorna a string convertida
    return buf;
}

/**
 * @brief   Concatena a string "source" no final da string "dest".
 *          Os tamanhos das duas strings SOMADOS está limitado a um máximo de
 *             max_len
 *          caracteres, incluindo o terminador nulo.
 *
 * @param   dest     String que irá receber a outra, no final.
 * @param   source   String que será adicionada no final da outra.
 *
 * @return  true se for bem sucedido ou false em caso de erro (falta terminador nulo?)
 */
bool stringNCat(char *dest, char *source, uint32_t max_len)
{
    int32_t i = 0;

    // Posiciona o ponteiro "dest" no terminador nulo
    i = 0;
    while (*dest && i < max_len)
    {
        i++;
        dest++;
    }
    if (i >= max_len)
    {
        return false;
    }

    // Adiciona a string "source" em "dest", obedecendo o limite de tamenho.
    i = 0;
    while (*source && i < max_len)
    {
        i++;
        *dest++ = *source++;
    }

    // Retorna true se não estourou o limite de tamanho
    return (i < max_len);
}

/**
 * @brief   Concatena a string "source" no final da string "dest".
 *          Os tamanhos das duas strings SOMADOS está limitado a um máximo de
 *             LOGICALIS_LONG_STRING_MAX_LENGTH
 *          caracteres, incluindo o terminador nulo.
 *
 * @param   dest     String que irá receber a outra, no final.
 * @param   source   String que será adicionada no final da outra.
 *
 * @return  true se for bem sucedido ou false em caso de erro (falta terminador nulo?)
 */
bool stringCat(char *dest, char *source)
{
    return stringNCat(dest, source, LOGICALIS_LONG_STRING_MAX_LENGTH);
}

/**
 * @brief   Retorna uma string que representa o valor booleano informado.
 *
 * @param   value    Valor booleano de entrada
 *
 * @return  "true" ou "false"
 */
char *boolToStr(bool value)
{
    static char *str_false = "false";
    static char *str_true = "true";

    if (value)
    {
        return str_true;
    }
    return str_false;
}

/**
 * @brief   Retorna uma string que representa o valor uint32 informado
 *          O tamanho da string retornada está limitado a
 *              LOGICALIS_SHORT_STRING_MAX_LENGTH
 *          caracteres, incluindo o terminador nulo.
 *
 * @param   value    Valor uint32 de entrada
 *
 * @return  uma string representando o valor informado.
 */
char *uint32ToStr(uint32_t value)
{
    static char buf[LOGICALIS_SHORT_STRING_MAX_LENGTH];
    int i = 0;
    int resto = 0;

    // Zera o buffer interno
    for (i = 0; i < LOGICALIS_SHORT_STRING_MAX_LENGTH; i++)
    {
        buf[i] = '\0';
    }

    // Converte os algarismos para caracteres (estará invertido: do menos para o mais significativo)
    i = 0;
    do
    {
        resto = value % 10;
        value /= 10;
        buf[i++] = (char) (resto + '0');
    } while (value && i < LOGICALIS_SHORT_STRING_MAX_LENGTH);
    --i;

    // Desinverte os algarismos, deixando do mais para o menos significativo
    for (int j = 0; j < i; j++, i--)
    {
        char tmp = buf[i];
        buf[i] = buf[j];
        buf[j] = tmp;
    }

    return buf;
}

/**
 * @brief   Retorna uma string que representa o valor int32 informado (ou NULL em caso de erro de acesso à RAM)
 *          O tamanho da string retornada está limitado a
 *              LOGICALIS_SHORT_STRING_MAX_LENGTH
 *          caracteres, incluindo o terminador nulo.
 *
 * @param   value    Valor int32 de entrada
 *
 * @return  uma string representando o valor informado.
 */
char *int32ToStr(int32_t value)
{
    static char buf[LOGICALIS_SHORT_STRING_MAX_LENGTH];

    // Zera o buffer interno
    for (int i = 0; i < LOGICALIS_SHORT_STRING_MAX_LENGTH; i++)
    {
        buf[i] = '\0';
    }

    // Adiciona o sinal de menos, se o número for negativo
    if (value < 0)
    {
        buf[0] = '-';
    }

    // Adiciona uma string que representa o módulo
    uint32_t value_abs = (value < 0 ? -value : value);
    if (
            !stringNCat(
                buf,
                uint32ToStr((uint32_t) value_abs),
                (value<0 ? LOGICALIS_SHORT_STRING_MAX_LENGTH-2 : LOGICALIS_SHORT_STRING_MAX_LENGTH-1)
            )
       )
    {
        return (char *) 0;
    }
    return buf;
}

/**
 * @brief   Retorna uma string que representa o valor float informado (ou NULL em caso de erro de acesso à RAM)
 *          O tamanho da string retornada está limitado a
 *              LOGICALIS_SHORT_STRING_MAX_LENGTH
 *          caracteres, incluindo o terminador nulo.
 *
 * @param   value    Valor int32 de entrada
 *
 * @return  uma string representando o valor informado.
 */
char *floatToStr(float value, int32_t decimal_digits)
{
    static char buf[LOGICALIS_SHORT_STRING_MAX_LENGTH];

    // Zera o buffer interno
    for (int i = 0; i < LOGICALIS_SHORT_STRING_MAX_LENGTH; i++)
    {
        buf[i] = '\0';
    }

    // Determina a parte inteira (com sinal)
    int32_t inteiro = (int32_t) value;

    // Determina a parte fracionária (módulo)
    uint32_t mult = 1;

    while (decimal_digits > 0)
    {
        mult *= 10;
        decimal_digits--;
    }

    float value_frac = value - (float) inteiro;
    uint32_t fracionario = fracionario = (value_frac < 0 ? (-value_frac) * mult : value_frac * mult);
    // Converte para string a parte inteira (incluindo sinal)
    if (!stringNCat(buf, int32ToStr(inteiro), LOGICALIS_SHORT_STRING_MAX_LENGTH-1))
    {
        return (char *) 0;
    }

    // Adiciona a parte fracionária (ou ".0" se não houver parte fracionária, para diferenciar de números inteiros)
    if (fracionario)
    {
        // Adiciona ponto decimal e parte fracionária
        if (!stringNCat(buf, ".", LOGICALIS_SHORT_STRING_MAX_LENGTH-1))
        {
            return (char *) 0;
        }
        if (!stringNCat(buf, uint32ToStr((uint32_t) fracionario), LOGICALIS_SHORT_STRING_MAX_LENGTH-1))
        {
            return (char *) 0;
        }
    }
    else
    {
        // Adiciona um ".0" para garantir que sistemas externos entendam que é fracionário
        if (!stringNCat(buf, ".0", LOGICALIS_SHORT_STRING_MAX_LENGTH-1))
        {
            return (char *) 0;
        }
    }
    return buf;
}



//==============================================================================
//
// Depuração
//
//==============================================================================

#if defined(LOGICALIS_DEBUG_STRING_TOOLS) && LOGICALIS_DEBUG_STRING_TOOLS

#include <stdio.h>

static void debug_lowercase()
{
    char buf_original[2] =
    { '\0', '\0' };
    char buf_lowercase[2] =
    { '\0', '\0' };

    printf("\n\nTeste: lowercase(): Espera-se que o \"Lowercase\" seja o formato minúsculo do \"Original\"");

    buf_original[0] = 'A';
    buf_lowercase[0] = lowercase(buf_original[0]);
    printf("\n   Original:  %s", buf_original);
    printf("\n   Lowercase: %s", buf_lowercase);

    buf_original[0] = 'a';
    buf_lowercase[0] = lowercase(buf_original[0]);
    printf("\n   Original:  %s", buf_original);
    printf("\n   Lowercase: %s", buf_lowercase);

    buf_original[0] = 'Z';
    buf_lowercase[0] = lowercase(buf_original[0]);
    printf("\n   Original:  %s", buf_original);
    printf("\n   Lowercase: %s", buf_lowercase);

    buf_original[0] = 'z';
    buf_lowercase[0] = lowercase(buf_original[0]);
    printf("\n   Original:  %s", buf_original);
    printf("\n   Lowercase: %s", buf_lowercase);

    buf_original[0] = '1';
    buf_lowercase[0] = lowercase(buf_original[0]);
    printf("\n   Original:  %s", buf_original);
    printf("\n   Lowercase: %s", buf_lowercase);

    buf_original[0] = '*';
    buf_lowercase[0] = lowercase(buf_original[0]);
    printf("\n   Original:  %s", buf_original);
    printf("\n   Lowercase: %s", buf_lowercase);
}

static void debug_strToLower()
{
    char *str1 = "minusculas 123456 !@#*";
    char *str2 = "MAIUSCULAS 123456 !@#*";
    char *msg = (void *) 0;

    printf("\n\nTeste: strToLower(): Espera-se que o \"strToLower\" seja o formato minúsculo do \"Original\"");

    msg = str1;
    printf("\n   Original:   %s", msg);
    printf("\n   strToLower: %s", strToLower(msg));

    msg = str2;
    printf("\n   Original:   %s", msg);
    printf("\n   strToLower: %s", strToLower(msg));
}

static void debug_boolToStr()
{
    printf("\n\nTeste: boolToStr(): Espera-se que o \"boolToStr\" apresente um string igual ao \"Original\"");

    printf("\n   Original:  false");
    printf("\n   boolToStr: %s", boolToStr(false));

    printf("\n   Original:  true");
    printf("\n   boolToStr: %s", boolToStr(true));
}

static void debug_uint32ToStr()
{
    char *msg_erro = "<NULL>";
    char *result = (void *) 0;
    uint32_t value = 0;

    printf("\n\nTeste: uint32ToStr(): Espera-se que a string \"uint32ToStr\" seja igual ao valor impresso em \"Original\"");

    value = 0;
    result = uint32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %u", value);
    printf("\n   uint32ToStr: %s", result);

    value = 1;
    result = uint32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %u", value);
    printf("\n   uint32ToStr: %s", result);

    value = 12;
    result = uint32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %u", value);
    printf("\n   uint32ToStr: %s", result);

    value = 123;
    result = uint32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %u", value);
    printf("\n   uint32ToStr: %s", result);

    value = 1234;
    result = uint32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %u", value);
    printf("\n   uint32ToStr: %s", result);

    value = 12345;
    result = uint32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %u", value);
    printf("\n   uint32ToStr: %s", result);

    value = 123456;
    result = uint32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %u", value);
    printf("\n   uint32ToStr: %s", result);
}

static void debug_int32ToStr()
{
    char *msg_erro = "<NULL>";
    char *result = (void *) 0;
    int32_t value = 0;

    printf("\n\nTeste: int32ToStr(): Espera-se que a string \"int32ToStr\" seja igual ao valor impresso em \"Original\"");

    value = -123456;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = -12345;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = -1234;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = -123;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = -12;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = -1;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = 0;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = 1;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = 12;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = 123;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = 1234;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = 12345;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);

    value = 123456;
    result = int32ToStr(value);
    if (!result)
        result = msg_erro;
    printf("\n   Original:    %i", value);
    printf("\n   int32ToStr:  %s", result);
}

static void debug_floatToStr()
{
    char *msg_erro = "<NULL>";
    char *result = (void *) 0;
    float value = 0;

    printf("\n\nTeste: floatToStr(): Espera-se que o string em \"floatToStr\" seja o valor definido (Necessário verificar source code para obter o valor entrado)");

    value = -1234;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = -123.4;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = -12.34;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = -1.234;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = 0;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = 1.234;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = 12.34;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = 123.4;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);

    value = 1234;
    result = floatToStr(value, 4);
    if (!result)
        result = msg_erro;
    printf("\n   floatToStr:  %s", result);
}

void debug_string_tools()
{
    debug_lowercase();
    debug_strToLower();
    debug_boolToStr();
    debug_uint32ToStr();
    debug_int32ToStr();
    debug_floatToStr();
}

#endif // LOGICALIS_DEBUG_STRING_TOOLS
