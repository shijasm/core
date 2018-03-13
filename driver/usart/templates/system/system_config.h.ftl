/* USART Driver Instance ${INDEX?string} Configuration Options */
#define DRV_USART_INDEX_${INDEX?string}                  ${INDEX?string}
#define DRV_USART_XMIT_QUEUE_SIZE_IDX${INDEX?string}     ${DRV_USART_TX_QUEUE_SIZE}
#define DRV_USART_RCV_QUEUE_SIZE_IDX${INDEX?string}      ${DRV_USART_RX_QUEUE_SIZE}
<#if DRV_USART_TX_DMA == true>
#define DRV_USART_XMIT_DMA_CH_IDX${INDEX?string}         DMA_CHANNEL_${DRV_USART_TX_DMA_CHANNEL}
</#if>
<#if DRV_USART_RX_DMA == true>
#define DRV_USART_RCV_DMA_CH_IDX${INDEX?string}          DMA_CHANNEL_${DRV_USART_RX_DMA_CHANNEL}
</#if>