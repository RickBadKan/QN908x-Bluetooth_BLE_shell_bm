// =============================================================================
/**
\file    ring_buffer.c
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
#include "ring_buffer.h"



//------------------------------------------------------------------------------
//
// Funções internas
//
//------------------------------------------------------------------------------

static int32_t ringbuf_indexCheck(ringbuf_t *pRingBuf, int32_t index);
static void ringbuf_IncIdxWrite(ringbuf_t *pRingBuf);
static void ringbuf_IncIdxRead(ringbuf_t *pRingBuf);

//
// Dado um índice "cru", verifica se ele estourou o tamanho do buffer.
// Se estourar, faz o efeito de buffer circular e recomeça a contar da posição 0
//
static int32_t ringbuf_indexCheck(ringbuf_t *pRingBuf, int32_t index)
{
	if(index >= pRingBuf->size)
		index -= pRingBuf->size;
	
	return index;
}

//
// Incrementa idxWrite e trata colisão com idxRead
//
static void ringbuf_IncIdxWrite(ringbuf_t *pRingBuf)
{
	// Avança idxWrite
	pRingBuf->idxWrite = (pRingBuf->idxWrite + 1 >= pRingBuf->size)  ? 0  : pRingBuf->idxWrite + 1;

	// Trata colisão com idxRead
	if(pRingBuf->idxWrite == pRingBuf->idxRead)
	{
		pRingBuf->idxRead = (pRingBuf->idxRead + 1 >= pRingBuf->size)  ? 0  : pRingBuf->idxRead + 1;

		// Avança o idxLock, caso esteja em uso e idxRead tenha colidido com ele
		if(pRingBuf->idxLock == pRingBuf->idxRead)
			pRingBuf->idxLock = (pRingBuf->idxLock + 1 >= pRingBuf->size)  ? 0  : pRingBuf->idxLock + 1;
	}
}

//
// Incrementa idxRead (caso o ringbuf não esteja vazio)
//
static void ringbuf_IncIdxRead(ringbuf_t *pRingBuf)
{
	if(pRingBuf->idxLock == -1)
	{
		if(pRingBuf->idxRead != pRingBuf->idxWrite)
			pRingBuf->idxRead = (pRingBuf->idxRead + 1 >= pRingBuf->size)  ? 0  : pRingBuf->idxRead + 1;
	}
	else
	{
		if(pRingBuf->idxRead != pRingBuf->idxLock)
			pRingBuf->idxRead = (pRingBuf->idxRead + 1 >= pRingBuf->size)  ? 0  : pRingBuf->idxRead + 1;
	}
}



//------------------------------------------------------------------------------
//
// API
//
//------------------------------------------------------------------------------

/**
 * @brief  Ajusta o estado inicial do ringbuf e o vincula ao pBuf informado.
 *
 * @param  pRingBuf  ponteiro para o ringbuf que está para ser configurado
 * @param  pBuf      ponteiro para um array que será usado pelo buffer circular
 * @param  bufSize   tamanho do array que será usado pelo buffer circular
 *
 * @return false se algum dos argumentos for inválido ou true em caso de sucesso.
 */
bool ringbuf_init(ringbuf_t *pRingBuf, uint8_t *pBuf, int32_t bufSize)
{
	if(!pRingBuf || !pBuf || !bufSize)
	{
		return false;
	}
	pRingBuf->size = bufSize;
	pRingBuf->pBuf = pBuf;
	pRingBuf->idxRead = 0;
	pRingBuf->idxWrite = 0;
	pRingBuf->idxLock = -1;

	return true;
}

//
// Esvazia o ringbuf.
//
void ringbuf_Clear(ringbuf_t *pRingBuf)
{
	pRingBuf->idxRead = 0;
	pRingBuf->idxWrite = 0;
	pRingBuf->idxLock = -1;
}

//
// Verifica se o ringbuf está vazio.
// Retorna:
//   0 - Não está vazio
//   1 - Está vazio
//
bool ringbuf_IsEmpty(ringbuf_t *pRingBuf)
{
	return pRingBuf->idxRead == pRingBuf->idxWrite;
}

//
// Retorna o total de bytes escritos no ringbuf, independentemente dele estar travado.
//
int32_t ringbuf_TotWriten(ringbuf_t *pRingBuf)
{
	// Caso buffer vazio
	if(pRingBuf->idxRead == pRingBuf->idxWrite)
		return 0;

	// Caso idxWrite após idxRead
	if(pRingBuf->idxWrite > pRingBuf->idxRead)
		return pRingBuf->idxWrite - pRingBuf->idxRead;

	// Caso idxWrite antes de idxRead (idxWrite já "deu a volta" no buffer circular)
	return pRingBuf->idxWrite + pRingBuf->size - pRingBuf->idxRead;
}

//
// Retorna o total de bytes escritos no ringbuf que podem ser lidos.
// Não conta os bytes travados.
//
int32_t ringbuf_TotReadable(ringbuf_t *pRingBuf)
{
	if( ringbuf_IsLocked(pRingBuf) )
	{
		// Caso buffer vazio
		if(pRingBuf->idxRead == pRingBuf->idxLock)
			return 0;

		// Caso idxLock após idxRead
		if(pRingBuf->idxLock > pRingBuf->idxRead)
			return pRingBuf->idxLock - pRingBuf->idxRead;

		// Caso idxLock antes de idxRead (idxLock já "deu a volta" no buffer circular)
		return pRingBuf->idxLock + pRingBuf->size - pRingBuf->idxRead;
	}
	else
	{
		// Caso buffer vazio
		if(pRingBuf->idxRead == pRingBuf->idxWrite)
			return 0;

		// Caso idxWrite após idxRead
		if(pRingBuf->idxWrite > pRingBuf->idxRead)
			return pRingBuf->idxWrite - pRingBuf->idxRead;

		// Caso idxWrite antes de idxRead (idxWrite já "deu a volta" no buffer circular)
		return pRingBuf->idxWrite + pRingBuf->size - pRingBuf->idxRead;
	}
}

//
// Retorna a capacidade de armazenamento do ringbuf, em bytes.
//
int32_t ringbuf_Capacity(ringbuf_t *pRingBuf)
{
	return pRingBuf->size - 1;
}

//
// Escreve dados no ringbuf.
//
void ringbuf_Write(ringbuf_t *pRingBuf, uint8_t *pInput, int32_t bytesToWrite)
{
	int32_t maxIterations = RINGBUF_MAX_SIZE;

	while(bytesToWrite-- && maxIterations--)
	{
		*(pRingBuf->pBuf + pRingBuf->idxWrite) = *pInput++;
		ringbuf_IncIdxWrite(pRingBuf);
	}
}

/*!
 * @brief  Adiciona uma string no ringbuffer (incluindo o terminador nulo)
 *
 * @param  pRingBuf  O ringbuffer
 * @param  str       String a ser adicionada no ringbuffer
 */
void ringbuf_WriteStr(ringbuf_t *pRingBuf, char *str)
{
	volatile int32_t maxIterations = RINGBUF_MAX_SIZE;

	// Adiciona a string
	while(*str && maxIterations--)
	{
		*(pRingBuf->pBuf + pRingBuf->idxWrite) = *str++;
		ringbuf_IncIdxWrite(pRingBuf);
	}

	// Adiciona o terminador nulo
	*(pRingBuf->pBuf + pRingBuf->idxWrite) = '\0';
	ringbuf_IncIdxWrite(pRingBuf);
}

//
// Escreve um byte no ringbuf.
//
void ringbuf_WriteByte(ringbuf_t *pRingBuf, uint8_t data)
{
	*(pRingBuf->pBuf + pRingBuf->idxWrite) = data;
	ringbuf_IncIdxWrite(pRingBuf);
}

//
// Lê e remove N bytes do ringbuf.
// Retorna
//   0 se o buffer estiver vazio ou se bytesToRead > ringbuf_TotReadable()
//   Ou o total de bytes efetivamente lidos
//
int32_t ringbuf_Read(ringbuf_t *pRingBuf, uint8_t *pOutput, int32_t bytesToRead)
{
	int32_t totReadBytes = 0;
	int32_t maxIterations = RINGBUF_MAX_SIZE;

	if(!ringbuf_IsEmpty(pRingBuf))
	{
		if(bytesToRead <= ringbuf_TotReadable(pRingBuf))
		{
			while(bytesToRead-- && maxIterations--)
			{
				*pOutput++ = *(pRingBuf->pBuf + pRingBuf->idxRead);
				ringbuf_IncIdxRead(pRingBuf);
				totReadBytes++;
			}
		}
	}
	return totReadBytes;
}

//
// Lê e remove um byte do ringbuf.
// Retorna
//   true se for bem sucedido
//   false se o buffer estiver vazio ou 0==ringbuf_TotReadable().
//
bool ringbuf_ReadByte(ringbuf_t *pRingBuf, uint8_t *pOutput)
{
	if( !ringbuf_IsEmpty(pRingBuf) )
	{
		if( ringbuf_TotReadable(pRingBuf) )
		{
			*pOutput = *(pRingBuf->pBuf + pRingBuf->idxRead);
			ringbuf_IncIdxRead(pRingBuf);
			return true;
		}
	}
	return false;
}

//
// Lê, mas não remove (peek = "olhadinha") um byte do ringbuf.
// Retorna true se for bem sucedido ou false se o buffer estiver vazio.
//
bool ringbuf_PeekByte(ringbuf_t *pRingBuf, uint8_t *pOutput)
{
	if(!ringbuf_IsEmpty(pRingBuf))
	{
		*pOutput = *(pRingBuf->pBuf + pRingBuf->idxRead);
		return true;
	}
	return false;
}

//
// Lê, mas não remove (peek = "olhadinha") um byte do ringbuf.
// Parâmetro "index" vai de 0 a ringbuf_TotWriten()-1
//
// Retorna:
//   true   se for bem sucedido
//   false  se o buffer estiver vazio ou o índice for inválido
//
bool ringbuf_PeekByteAt(ringbuf_t *pRingBuf, uint8_t *pOutput, int32_t index)
{
	if(!ringbuf_IsEmpty(pRingBuf))
	{
		if(index < ringbuf_TotWriten(pRingBuf))
		{
			*pOutput = *(pRingBuf->pBuf + ringbuf_indexCheck(pRingBuf, pRingBuf->idxRead + index));
			return true;
		}
	}
	return false;
}

//
// Remove N bytes do ringbuf.
// Retorna:
//   true   se for bem sucedido
//   false  se o buffer estiver vazio ou se o total a remover for superior a ringbuf_TotReadable()
//
bool ringbuf_Delete(ringbuf_t *pRingBuf, int32_t bytesToDelete)
{
	if(!ringbuf_IsEmpty(pRingBuf))
	{
		if(bytesToDelete <= ringbuf_TotReadable(pRingBuf))
		{
			int32_t maxIterations = RINGBUF_MAX_SIZE;
			while(bytesToDelete-- && maxIterations--)
			{
				ringbuf_IncIdxRead(pRingBuf);
			}
			return true;
		}
	}
	return false;
}

//
// Remove um byte do ringbuf.
// Retorna:
//   true   se for bem sucedido
//   false  se o buffer estiver vazio ou se ringbuf_TotReadable() for igual a 0
//
bool ringbuf_DeleteByte(ringbuf_t *pRingBuf)
{
	if(!ringbuf_IsEmpty(pRingBuf))
	{
		if( ringbuf_TotReadable(pRingBuf) )
		{
			ringbuf_IncIdxRead(pRingBuf);
			return true;
		}
	}
	return false;
}

//
// Atualiza N bytes no ringbuf.
// Retorna:
//   true   se for bem sucedido
//   false  se o buffer estiver vazio ou se totNewValues > ringbuf_TotReadable()
//
bool ringbuf_Update(ringbuf_t *pRingBuf, uint8_t *pNewValues, int32_t totNewValues)
{
	return ringbuf_UpdateAt(pRingBuf, pNewValues, totNewValues, 0);
}

//
// Atualiza N bytes no ringbuf, a partir da posição informada.
// Retorna:
//   true   se for bem sucedido
//   false  se o buffer estiver vazio ou se (index+totNewValues) > ringbuf_TotReadable()
//
bool ringbuf_UpdateAt(ringbuf_t *pRingBuf, uint8_t *pNewValues, int32_t totNewValues, int32_t index)
{
	if(!ringbuf_IsEmpty(pRingBuf))
	{
		if((index+totNewValues) <= ringbuf_TotReadable(pRingBuf))
		{
			totNewValues += pRingBuf->idxRead + index;
			for(int i = pRingBuf->idxRead + index; i < totNewValues; i++)
			{
				*(pRingBuf->pBuf + ringbuf_indexCheck(pRingBuf, i)) = *pNewValues++;
			}
			return true;
		}
	}
	return false;
}

//
// Atualiza um byte no ringbuf.
// Retorna:
//   true   se for bem sucedido
//   false  se o buffer estiver vazio
//
bool ringbuf_UpdateByte(ringbuf_t *pRingBuf, uint8_t newValue)
{
	return ringbuf_UpdateByteAt(pRingBuf, newValue, 0);
}

//
// Atualiza um byte no ringbuf, na posição informada.
// Retorna:
//   true   se for bem sucedido
//   false  se o buffer estiver vazio ou se index >= ringbuf_TotReadable()
//
bool ringbuf_UpdateByteAt(ringbuf_t *pRingBuf, uint8_t newValue, int32_t index)
{
	if(!ringbuf_IsEmpty(pRingBuf))
	{
		if( index < ringbuf_TotReadable(pRingBuf) )
		{
			*(pRingBuf->pBuf + ringbuf_indexCheck(pRingBuf, pRingBuf->idxRead + index)) = newValue;
			return true;
		}
	}
	return false;
}

//
// Aplica uma trava de leitura no buffer, que te um de dois efeitos:
//   Se idxWrite == -1 (estado inicial de um novo buffer) então
//     idxLock <-- 0
//   Senão
//     idxLock <-- idxWrite
//   Fim Se
//
// Uma vez habilitada a trava, idxRead fica limitado a até "idxLock-1".
//
// O conteúdo escrito após travar o buffer poderá ser descartado ou
// consolidado, conforme a lógica da aplicação.
//
// IMPORTANTE:
//   Ative a trava imediatamente antes de escrever o 1o byte
//   da sequência que precisará ficar travada.
void ringbuf_Lock(ringbuf_t *pRingBuf)
{
	if(pRingBuf->idxWrite == -1)
		pRingBuf->idxLock = 0;
	else
		pRingBuf->idxLock = pRingBuf->idxWrite;
}

//
// Remove a trava de leitura do buffer:
//   idxLock <-- -1
//   idxRead fica livre para varrer completamente o buffer
// Isto é semelhante a um commit de bancos de dados
//
// A trava é útil para armazenar dados num buffer primário e impedir que esses dados
// passem para um buffer secundário antes de serem validados.
//
void ringbuf_Unlock(ringbuf_t *pRingBuf)
{
	pRingBuf->idxLock = -1;
}

//
// Descarta o conteúdo que estava "locked" no buffer e remove a trava
// Isto é semelhante a um rollback de bancos de dados
//
// A trava é útil para armazenar dados num buffer primário e impedir que esses dados
// passem para um buffer secundário antes de serem validados.
//
void ringbuf_DiscardLock(ringbuf_t *pRingBuf)
{
	if(pRingBuf->idxLock > -1)
		pRingBuf->idxWrite = pRingBuf->idxLock;
	pRingBuf->idxLock = -1;
}

//
// Retorna true se o buffer estiver com uma trava de leitura
//
// A trava é útil para armazenar dados num buffer primário e impedir que esses dados
// passem para um buffer secundário antes de serem validados.
//
bool ringbuf_IsLocked(ringbuf_t *pRingBuf)
{
	return (pRingBuf->idxLock != -1);
}
