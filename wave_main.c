#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct WaveHeader   // Header do .wav
{
    char chunkID[4];
    DWORD chunkSize;
    char format[4];
    char subchunk1ID[4];
    DWORD subchunk1Size;// infos sobre formato
    DWORD subchunk2Size; // tamanho dos dados de audio
    WAVEFORMATEX waveFormat;
    DWORD nAvgBytesPerSec;
};

infos(char *nomearq)
{
    FILE* file;
    struct WaveHeader cabecalho;

    file = fopen(nomearq, "r");

    fread(&cabecalho, sizeof(struct WaveHeader), 1, file);

    //printa as informacoes basicas do arquivo
    printf("Nome do arquivo: %s\n", nomearq);
    printf("Informacoes basicas do arquivo .wav:\n");
    printf("Bits por amostra (resolucao do audio): %d\n", cabecalho.waveFormat.wBitsPerSample); // 8 bits - baixa qualidade, 24 a 32 bits - alta qualidade
    printf("Taxa de amostragem: %ld\n", cabecalho.waveFormat.nSamplesPerSec);
    printf("Numero de canais (1 - mono, 2 - estereo): %d\n", cabecalho.waveFormat.nChannels);
}

void recortarAudio(char *nomearq, int inicio_ms, int fim_ms)
{
    FILE *file, *fileOut;
    struct WaveHeader cabecalho;
    int tamanho_recorte_ms, tamanho_recorte_bytes;
    int bytes_para_pular;

    file = fopen(nomearq, "rb");
    fileOut = fopen("audio_recortado.wav", "wb");

    if (file == NULL || fileOut == NULL) {
        printf("O arquivo nao foi encontrado ou nao existe.");
        return 0;
    }

    fread(&cabecalho, sizeof(struct WaveHeader), 1, file);
    fwrite(&cabecalho, sizeof(struct WaveHeader), 1, fileOut);

    tamanho_recorte_ms = fim_ms - inicio_ms; // intervalo do recorte
    tamanho_recorte_bytes = tamanho_recorte_ms * cabecalho.waveFormat.nAvgBytesPerSec / 1000; // nAvgBytesPerSec = taxa de bytes por sec

    // Atualiza o tamanho do chunk e do arquivo no cabeçalho
    cabecalho.chunkSize = tamanho_recorte_bytes + sizeof(struct WaveHeader) - 8; //  tamanho total do chunk principal (que inclui todo o arquivo, menos os primeiros 8 bytes
    cabecalho.subchunk2Size = tamanho_recorte_bytes;

    fseek(file, sizeof(struct WaveHeader), SEEK_SET); // Vai para o início dos dados de áudio

    bytes_para_pular = inicio_ms * cabecalho.waveFormat.nAvgBytesPerSec / 1000; // altera o tempo de inicio
    fseek(file, bytes_para_pular, SEEK_CUR);

    char *dados_audio = (char *)malloc(tamanho_recorte_bytes);
    fread(dados_audio, sizeof(char), tamanho_recorte_bytes, file);
    fwrite(dados_audio, sizeof(char), tamanho_recorte_bytes, fileOut);

    free(dados_audio);
    fclose(file);
    fclose(fileOut);

    printf("Audio recortado com sucesso!\n");
    PlaySound("audio_recortado.wav", NULL, SND_FILENAME | SND_SYNC);


}

int main()
{
    FILE* file;
    struct WaveHeader cabecalho;
    int opcao;
    char nomearq[50];
    int inicio, fim;
    int recorte;

    //pergunta ao usuario o nome do arquivo e acrescenta .wav
    printf("Digite o nome do arquivo: ");
    scanf("%s", nomearq);
    strcat(nomearq, ".wav");

    file = fopen(nomearq, "r");

    if (file == NULL)
    {
        printf("O arquivo nao foi encontrado ou nao existe.");
        fclose(file);
        return 0;
    }

    infos(nomearq);

    do
    {
        printf("\nMenu\n");
        printf("0 - Sair\n");
        printf("1 - Reproduzir o audio\n");
        printf("2 - Recortar e reproduzir o audio\n");
        printf("Selecione o que deseja fazer:\n");
        scanf("%d", &opcao);

        switch (opcao) {
        case 0:
            printf("Saindo...\n");
            break;
        case 1:
            printf("Reproduzindo o audio...\n");
            PlaySound(nomearq, NULL, SND_FILENAME | SND_SYNC); // reproduz o arquivo
            break;
        case 2:
            printf("Digite o tempo de inicio do recorte (em segundos): ");
            scanf("%d", &inicio);
            printf("Digite o tempo de fim do recorte (em segundos): ");
            scanf("%d", &fim);
            recortarAudio(nomearq, inicio*1000, fim*1000);
            break;
        default:
            printf("Opcao invalida.\n");
            break;
    }

        }while(opcao != 0);

    return 0;

    fclose(file);
}
