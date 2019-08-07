// =============================================================================
/**
\file    string_tools.h
\brief   Ferramentas de propósito geral para manipulação de strings.

\details
   Biblioteca de propósito geral para manipulação de string e conversão de
   valores numéricos para strings.

   Para habilitar a rotina de depuração, veja a constante:
     LOGICALIS_DEBUG_STRING_TOOLS

\b@{Histórico de Alterações:@}
	- 2019.03.18 -- alteração de retorno da func debug_string_tools de bool para void
				 (v1.0.3)
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
#ifndef H_STRING_TOOLS
#define H_STRING_TOOLS

#include <stdint.h>
#include <stdbool.h>



//==============================================================================
//
// Constantes
//
//==============================================================================

/**
 * @brief  Habilita (1) ou desabilita (0) a função de depuração desta lib
 */
#define LOGICALIS_DEBUG_STRING_TOOLS          1

/**
 * @brief  Tamanho máximo de uma string longa (para evitar estouro de memória)
 */
#define LOGICALIS_LONG_STRING_MAX_LENGTH   2048

/**
 * @brief  Tamanho máximo de uma string média (protege contra estouro de memória)
 */
#define LOGICALIS_MEDIUM_STRING_MAX_LENGTH   96

/**
 * @brief  Tamanho máximo de uma string curta (protege contra estouro de memória)
 */
#define LOGICALIS_SHORT_STRING_MAX_LENGTH    32



//==============================================================================
//
// API
//
//==============================================================================
#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus


//
// String Tools
//
char lowercase(char c);
char *strToLower(char *s);
bool stringNCat(char *dest, char *source, uint32_t max_len);
bool stringCat(char *dest, char *source);
char *boolToStr(bool value);
char *uint32ToStr(uint32_t value);
char *int32ToStr(int32_t value);
char *floatToStr(float value, int32_t decimal_digits);

//
// Depuração
//
#if defined(LOGICALIS_DEBUG_STRING_TOOLS) && LOGICALIS_DEBUG_STRING_TOOLS
void debug_string_tools();
#endif // LOGICALIS_DEBUG_STRING_TOOLS


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // H_STRING_TOOLS
