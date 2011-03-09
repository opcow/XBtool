#ifndef TEA_HASH_H
#define	TEA_HASH_H


typedef unsigned char TEA_HASH[16];

extern	void	calc_TEA_hash(unsigned char *p_base, unsigned int count, unsigned int ival1, unsigned int ival2, TEA_HASH *p_hash);

extern	int	is_expected_FBL_TEA_hash(TEA_HASH *p_hash);

#define	FBL_TEA_HASH_IVAL1	0xffffd400
#define	FBL_TEA_HASH_IVAL2	0xfffffc80


#endif	// TEA_HASH_H
