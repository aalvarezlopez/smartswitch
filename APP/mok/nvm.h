#ifndef NVM_MOCK_H
#define NVM_MOCK_H
typedef struct nvmSettings_s{
	uint32_t header[3];
	uint8_t ip[4];
	uint16_t id;
}nvmSettings_st;
#endif
