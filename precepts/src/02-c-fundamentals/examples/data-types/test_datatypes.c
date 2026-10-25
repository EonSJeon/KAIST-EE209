/*--------------------------------------------------------------------*/
/* test_datatypes.c                                                   */
/* Author: Younghwan Go                                               */
/*--------------------------------------------------------------------*/

#include <stdio.h>

void test_char(void)
{
	/* Print character 'a' & 'a' + 1 */
	printf("Print character 'a': %c", 'a');
	printf("\n");
	printf("Print character 'a' + 1: %c", 'a' + 1);
	printf("\n");

	/* Print tab */
	printf("Print%cwith%ctab%cgood", '\t', '\t', '\t');
	printf("\n");
	printf("Print\twith\ttab\tgood");
	printf("\n");

	/* Print newline */
	printf("Print newline: %c", '\n');
	printf("Print newline: \n");
}

void test_short(void)
{
	short s_val = 32767;
	unsigned short u_val = 32768;
	
	/* print 32768 as signed vs. unsigned short */
	printf("Print \"32768\" as signed: %hd\n", s_val + 1);
	printf("Print \"32768\" as unsigned: %hu\n", u_val);
	
	/* print 32768 - 1 as signed vs. unsigned short */
	printf("Print \"32768-1\" as signed: %hd\n", s_val);
	printf("Print \"32768-1\" as unsigned: %hu\n", u_val - 1);
}

void test_int(void)
{
	int s_val = 2147483647;
	unsigned int u_val = 2147483648;
	
	/* print 2147483648 as signed vs. unsigned int */
	printf("Print \"2147483648\" as signed: %d\n", s_val + 1);
	printf("Print \"2147483648\" as unsigned: %u\n", u_val);
	
	/* print 2147483648 - 1 as signed vs. unsigned int */
	printf("Print \"2147483648-1\" as signed: %d\n", s_val);
	printf("Print \"2147483648-1\" as unsigned: %u\n", u_val - 1);
}

void test_long(void)
{
	long s_val = 9223372036854775807L;
	unsigned long u_val = 9223372036854775808UL;
	
	/* print 2^63 as signed vs. unsigned long */
	printf("Print \"2^63\" as signed: %ld\n", s_val + 1);
	printf("Print \"2^63\" as unsigned: %lu\n", u_val);
	
	/* print 2^63 - 1 as signed vs. unsigned long */
	printf("Print \"2^63-1\" as signed: %ld\n", s_val);
	printf("Print \"2^63-1\" as unsigned: %lu\n", u_val - 1);
}

void test_float(void)
{
	float f_val = 1345.41F;
	float s_val = 13.4541E2F;

	/* print fixed notation */
	printf("Print \"1345.41F\": %f\n", f_val);
	printf("Print \"1345.41F\" up to two decimal digits: %.2f\n", f_val);

	/* print scientific notation */
	printf("Print \"13.4541E2F\": %f\n", s_val);
	printf("Print \"13.4541E2F\" up to two decimal digits: %.2f\n", s_val);
}

void test_double(void)
{
	double d_val = 1345.41;
	double s_val = 13.4541E2;

	/* print fixed notation */
	printf("Print \"1345.41\": %f\n", d_val);
	printf("Print \"1345.41\" up to two decimal digits: %.2f\n", d_val);

	/* print scientific notation */
	printf("Print \"13.4541E2\": %f\n", s_val);
	printf("Print \"13.4541E2\" up to two decimal digits: %.2f\n", s_val);
}

void test_long_double(void)
{
	long double ld_val = 1345.41L;
	long double s_val = 13.4541E2L;

	/* print fixed notation */
	printf("Print \"1345.41L\": %Lf\n", ld_val);
	printf("Print \"1345.41L\" up to two decimal digits: %.2Lf\n", ld_val);

	/* print scientific notation */
	printf("Print \"13.4541E2L\": %Lf\n", s_val);
	printf("Print \"13.4541E2L\" up to two decimal digits: %.2Lf\n", s_val);
}

int main(void)
{
	/* char test */
	printf("======== test char ========\n");
	test_char();

	/* short test */
	printf("\n======== test short ========\n");
	test_short();

	/* int test */
	printf("\n======== test int ========\n");
	test_int();

	/* long test */
	printf("\n======== test long ========\n");
	test_long();

	/* float test */
	printf("\n======== test float ========\n");
	test_float();

	/* double test */
	printf("\n======== test double ========\n");
	test_double();

	/* long double test */
	printf("\n======== test long double ========\n");
	test_long_double();

	return 0;
}
