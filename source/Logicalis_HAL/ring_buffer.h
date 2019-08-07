// =============================================================================
/**
\file    ring_buffer.h
\brief   Buffer circular em RAM.

\details
   Esta biblioteca implementa um buffer circular em memória RAM. Este tipo
   de buffer é especialmente útil em autômatos de protocolos de comunicação.

\author
  Wagner A. P. Coimbra

\b@{Princípio de Operação:@}

    Definições:
      - O buffer circular é um array que possui dois índices associados:
          - Um para leitura (idxRead) e
          - Outro para escrita (idxWrite)

      - idxWrite aponta para a posição logo após à do último byte escrito

      - idxRead aponta para o byte mais antigo escrito no buffer

      - Quando o buffer está vazio, idxWrite == idxRead

      - Quando um byte é escrito no buffer, idxWrite é incrementado. Esse incremento
        pode fazer com que idxWrite coincida com idxRead (isto ocorre quando o
        buffer fica cheio). Como o estado de idxRead == idxWrite indica buffer vazio,
        sempre que o buffer enche e os índices coincidem, idxRead precisa ser
        incrementado, o que descarta o byte mais antigo.

      - Quando um byte é lido do buffer, idxRead é incrementado. Esse incremento
        poderia fazer com que idxRead coincidisse com idxWrite. Mas como a posição
        de indicada por idxWrite representa um byte "vazio", o valor máximo para
        idxRead é idxWrite-1.

      - A capacidade máxima de armazenamento do buffer circular é TAMANHO ALOCADO - 1

    Análise de casos:
      Buffer vazio (idxRead == idxWrite)
        0 1 2 3 4 5 6 7 8 9
        - - - - - - - - - -
        r
        w

      Buffer cheio (RING_BUFFER_SIZE - 1 bytes escritos)
        0 1 2 3 4 5 6 7 8 9
        ! ! ! ! ! ! ! ! ! -
        r
                          w

      Buffer parcialmente cheio, w > r
        0 1 2 3 4 5 6 7 8 9
        ! ! ! ! ! - - - - -    ==> total escrito = w - r + 1
        r
                  w

      Buffer parcialmente cheio, r > w
        0 1 2 3 4 5 6 7 8 9    TOTBYTES = 10
        ! - - - - ! ! ! ! !    ==> total escrito = w + TOTBYTES - r
          w
                  r

\b@{Histórico de Alterações:@}

    - 2018.10.04 -- A função:
                      ringbuf_Init()
                    passou a ser chamada de:
                      ringbuf_init()  -  (começa com letra minúscula, este
                                          padrão está aplicado à maioria das
                                          funcções implementadas e vai, com
                                          o tempo, ser aplicado em todas!)
                    e passou a retornar bool (antes era void) para indicar
                    o status de êxito da configuração incial.
                    (v1.0.7)
    - 2018.10.01 -- Criada a função
                      ringbuf_WriteStr()
                    (v1.0.6)
    - 2017.10.06 -- Deixaram de ser inline e se tornaram funções comuns:
                      ringbuf_Lock()
                      ringbuf_Unlock()
                      ringbuf_DiscardLock()
                      ringbuf_IsLocked()
                      ringbuf_WriteByte()
                    Isto foi feito para tornar privadas as funções:
                      ringbuf_IncIdxWrite()
                      ringbuf_IncIdxRead()
                 -- Foram criadas as funções:
                      ringbuf_ReadByte()
                      ringbuf_PeekByte()
                      ringbuf_PeekByteAt()
                      ringbuf_Delete()
                      ringbuf_DeleteByte()
                      ringbuf_Update()
                      ringbuf_UpdateAt()
                      ringbuf_UpdateByte()
                      ringbuf_UpdateByteAt()
                    (v1.0.5)
    - 2017.09.11 -- Adicionada regra para idxWrite==-1 em:
                      ringbuf_Lock()
                    (v1.0.4)
    - 2017.09.09 -- Criada a função:
                      ringbuf_TotReadable()
                    (v1.0.3)
    - 2017.09.05 -- Criadas as funções:
                      void ringbuf_Lock(ringbuf_t *pRingBuf)
                      void ringbuf_Unlock(ringbuf_t *pRingBuf)
                      void ringbuf_DiscardLock(ringbuf_t *pRingBuf)
                      bool ringbuf_IsLocked(ringbuf_t *pRingBuf)
                    que tiveram impacto sobre as funções:
                      void ringbuf_IncIdxWrite(ringbuf_t *pRingBuf)
                      void ringbuf_IncIdxRead(ringbuf_t *pRingBuf)
                    e sobre a estrutura:
                      ringbuf_t (adicionado campo int32_t idxLock)
                    (v1.0.2)
    - 2017.08.29 -- Prefixos alterados para "ringbuf_", para padronizar com as demais
                    bibliotecas.
                    (v1.0.1)
    - 2017.08.06 -- Primeira versão (v1.0.0)
*/
// =============================================================================
#ifndef H_RING_BUFFER
#define H_RING_BUFFER

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif // _cplusplus



//------------------------------------------------------------------------------
//
// Constantes
//
//------------------------------------------------------------------------------

//
// Tamanho máximo recomendado para um ringbuffer.
// É usado em laços para evitar travamentos.
//
#define RINGBUF_MAX_SIZE 16384u



//------------------------------------------------------------------------------
//
// Tipos e estruturas de dados
//
//------------------------------------------------------------------------------

//
// Definição de um RingBuffer.
//
// O campo pBuf aponta para um array externo à estrutura.
//
// O uso de estrutura com ponteiro para o buffer ao invpes de constantes e
// variáveis globais permite que RingBuffers com diferentes tamanhos sejam
// criados em tempo de execução.
//
typedef struct ringbuf
{
	int32_t size;     // tamanho da área em pBuf
	uint8_t *pBuf;    // área de armazenamento em RAM
	int32_t idxRead;  // Índice de leitura.
	int32_t idxWrite; // Índice de escrita.
	int32_t idxLock;  // Índice de lock. -1 significa sem lock. > -1 significa que pode haver leitura até esse índice.
} ringbuf_t;



//------------------------------------------------------------------------------
//
// Protótipos
//
//------------------------------------------------------------------------------

//
// Configuração inicial
//
bool ringbuf_init(ringbuf_t *pRingBuf, uint8_t *pBuf, int32_t bufSize);

//
// Gerenciamento
//
void ringbuf_Clear(ringbuf_t *pRingBuf);
bool ringbuf_IsEmpty(ringbuf_t *pRingBuf);
int32_t ringbuf_TotWriten(ringbuf_t *pRingBuf);
int32_t ringbuf_TotReadable(ringbuf_t *pRingBuf);
int32_t ringbuf_Capacity(ringbuf_t *pRingBuf);

//
// Leitura e escrita
//
void ringbuf_Write(ringbuf_t *pRingBuf, uint8_t *pInput, int32_t bytesToWrite);
void ringbuf_WriteStr(ringbuf_t *pRingBuf, char *str);
void ringbuf_WriteByte(ringbuf_t *pRingBuf, uint8_t data);

int32_t ringbuf_Read(ringbuf_t *pRingBuf, uint8_t *pOutput, int32_t bytesToRead);
bool ringbuf_ReadByte(ringbuf_t *pRingBuf, uint8_t *pOutput);

bool ringbuf_PeekByte(ringbuf_t *pRingBuf, uint8_t *pOutput);
bool ringbuf_PeekByteAt(ringbuf_t *pRingBuf, uint8_t *pOutput, int32_t index);

bool ringbuf_Delete(ringbuf_t *pRingBuf, int32_t bytesToDelete);
bool ringbuf_DeleteByte(ringbuf_t *pRingBuf);

bool ringbuf_Update(ringbuf_t *pRingBuf, uint8_t *pNewValues, int32_t totNewValues);
bool ringbuf_UpdateAt(ringbuf_t *pRingBuf, uint8_t *pNewValues, int32_t totNewValues, int32_t index);
bool ringbuf_UpdateByte(ringbuf_t *pRingBuf, uint8_t newValue);
bool ringbuf_UpdateByteAt(ringbuf_t *pRingBuf, uint8_t newValue, int32_t index);

//
// Locking (apoio à consolidação e ao descarte de dados)
//
void ringbuf_Lock(ringbuf_t *pRingBuf);
void ringbuf_Unlock(ringbuf_t *pRingBuf);
void ringbuf_DiscardLock(ringbuf_t *pRingBuf);
bool ringbuf_IsLocked(ringbuf_t *pRingBuf);


#if defined(__cplusplus)
}
#endif

#endif // H_RING_BUFFER
