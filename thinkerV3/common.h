#pragma once
//
//	common.h
//	thinkerV3�S�\�[�X���ʂŎg�p������̂��`����w�b�_�t�@�C��
// 

// �f�o�b�O�}�N��
#ifdef _DEBUG
#define LOGOUT_INIT(level, path)	logging.init(level, path)
#define LOGOUT(level, format, ...)	logging.logout(level, format, __VA_ARGS__)
#define LOGOUT_FLUSH()				logging.flush()
#define	LOGOUT_END()				logging.end()
#else
#define LOGOUT_INIT(level, path)
#define LOGOUT(level, format, ...)
#define LOGOUT_FLUSH()
#define	LOGOUT_END()
#endif
