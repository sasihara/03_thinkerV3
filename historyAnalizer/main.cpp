#include <stdio.h>
#include "logging.h"
#include "history.hpp"
#include "thinkV1.hpp"

#define FORMAT_VERSION_SUPPORTED	200

int main(int argc, char** argv)
{
	int ret;

	// ���s�o��
	printf("\n");

	// ���̓`�F�b�N
	if (argc < 2) {
		printf("historyAnalizer.exe [�q�X�g���t�@�C����]\n\n");
		return -1;
	}

	// ���̓t�@�C���̃I�[�v��
	FILE* fIn;
	if (fopen_s(&fIn, argv[1], "rb") != 0) {
		printf("�q�X�g���t�@�C�� %s ��������܂���.\n\n", argv[1]);
		return -2;
	}
	else {
		printf("���O�t�@�C����: %s\n", argv[1]);
	}

	// ���O�t�@�C����
	char logFileName[1024];

	// ���O����&�o��
	Logging logging;
	HistoryData historyData;

	try {
		// �o�̓t�@�C���̃I�[�v��
		// �o�̓t�@�C�����̐���
		// �t�@�C�����̒������擾
		size_t len = strlen(argv[1]);

		// �g���q�`�F�b�N
		if (strcmp(&argv[1][len - 4], ".bhs") != 0) {
			throw -3;
		}

		// �g���q�̒u�����������̂�V�t�@�C�����Ƃ���
		errno_t errno_t_ret;
		errno_t_ret = strcpy_s(logFileName, sizeof(logFileName), argv[1]);
		if (errno_t_ret != 0) throw -4;

		errno_t_ret = strcpy_s(&logFileName[len - 4], 5, ".txt");
		if (errno_t_ret != 0) throw -5;

		// ���O�t�@�C�����̉�ʏo��
		printf("�o�̓t�@�C����: %s\n", logFileName);

		// ���O�t�@�C���̃I�[�v��
		logging.init(9, logFileName);
	}
	catch (int ret) {
		printf("\n[ERROR] %s�̃I�[�v���Ɏ��s.\n\n", logFileName);

		fclose(fIn);
		return ret;
	}

	try {
		// �t�@�C���ǂݍ���
		int c = 0;

		int sum = 0;

		size_t count;
		unsigned _int16 formatVersion;

		count = fread_s(&formatVersion, sizeof(formatVersion), sizeof(formatVersion), 1, fIn);

		if (formatVersion != FORMAT_VERSION_SUPPORTED) {
			printf("\n[ERROR] �T�|�[�g����ĂȂ��t�H�[�}�b�g�̃t�@�C���ł�(�o�[�W���� = %d)\n", formatVersion);
			throw - 1;
		}

		while (feof(fIn) == 0) {
			//logging.logout("%d(0x%X)�o�C�g�ڂ���%d�o�C�g�ǂݍ���", sum, sum, sizeof(historyData.diskcolor));
			count = fread_s(&historyData.diskcolor, sizeof(historyData.diskcolor), sizeof(historyData.diskcolor), 1, fIn);
			if (count < 1) break;
			sum += sizeof(historyData.diskcolor);

			//logging.logout("%d(0x%X)�o�C�g�ڂ���%d�o�C�g�ǂݍ���", sum, sum, sizeof(historyData.board));
			count = fread_s(historyData.board, sizeof(historyData.board), sizeof(historyData.board), 1, fIn);
			if (count < 1) break;
			sum += sizeof(historyData.board);

			//logging.logout("%d(0x%X)�o�C�g�ڂ���%d�o�C�g�ǂݍ���", sum, sum, sizeof(historyData.probability));
			count = fread_s(historyData.probability, sizeof(historyData.probability), sizeof(historyData.probability), 1, fIn);
			if (count < 1) throw -7;
			sum += sizeof(historyData.probability);

			//logging.logout("%d(0x%X)�o�C�g�ڂ���%d�o�C�g�ǂݍ���", sum, sum, sizeof(historyData.value));
			count = fread_s(&historyData.value, sizeof(historyData.value), sizeof(historyData.value), 1, fIn);
			if (count < 1) throw -8;
			sum += sizeof(historyData.value);

			// board���e�̏o��
			c = c + 1;
			logging.logout("��No. %d", c);
			logging.logprintf("�v���C���[�̐΂̐F: %s\n\n", historyData.diskcolor == DISKCOLORS::COLOR_BLACK ? "��" : historyData.diskcolor == DISKCOLORS::COLOR_WHITE ? "��" : "");

			ret = logoutBoard(logging, historyData.board);

			// probalibity�̏o��
			logging.logprintf("\n*** Probability ***\n");
			for (size_t x = 0; x < 8; x++) {
				for (size_t y = 0; y < 8; y++) {
					if (historyData.board[y * 8 + x] == DISKCOLORS::COLOR_BLACK) {
						logging.logprintf("��", historyData.probability[y * 8 + x]);
					}
					else if (historyData.board[y * 8 + x] == DISKCOLORS::COLOR_WHITE) {
						logging.logprintf("��", historyData.probability[y * 8 + x]);
					}
					else {
						logging.logprintf("%2.0f", historyData.probability[y * 8 + x] * 100);
					}
					logging.logprintf(",");
				}

				logging.logprintf("\n");
			}
			logging.logprintf("\n");

			logging.logprintf("(���f�[�^)\n");
			for (size_t i = 0; i < 65; i++) {
				logging.logprintf("%.02f", historyData.probability[i]);
				if (i % 8 == 7) {
					logging.logprintf("\n");
				}
				else {
					logging.logprintf(", ");
				}
			}
			logging.logprintf("\n\n");

			// value�l�̏o��
			logging.logprintf("*** Value ***\n");
			logging.logprintf("%f\n\n", historyData.value);
		}

		// �t�@�C���̃N���[�Y
		fclose(fIn);
		logging.end();

		// ��ʏo��
		printf("�o�͏I��.\n");

		return 0;
	}
	catch (int ret) {
		// ��ʏo��
		printf("\n[ERROR] �t�@�C���ǂݍ��݂ŗ�O�������B��ONo. = %d.\n", ret);

		// ���O�o��
		logging.logout("[ERROR] �t�@�C���ǂݍ��݂ŗ�O�������B��ONo. = %d.", ret);

		// �t�@�C���̃N���[�Y
		fclose(fIn);
		logging.end();
		return ret;
	}
}