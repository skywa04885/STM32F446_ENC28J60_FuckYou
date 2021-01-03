#include "dns.h"

u8 *dns_add_question(u8 *p, const char *qname, dns_qtype_t qtype, dns_qclass_t qclass)
{
	u8 start = 0, end = 0;

	dns_label_seg_t *seg = (dns_label_seg_t *) p;
	for (const char *c = qname; ; ++c)
	{
		if (*c == '.' || *c == '\0')
		{
			seg->len = start - end;

			memcpy((void *) seg->next, (void *) &qname[start], end - start);
			seg = (dns_label_seg_t *) &seg->next[end - start];

			start = ++end;

			if (*c == '\0') break;
			continue;
		}

		++end;
	}

//
//	for (const char *c = qname; ; ++c)
//	{
//		if (*c == '.' || *c == '\0')
//		{
//			*p = end - start;
//			++p;
//
//			// Copies the ASCII part of the QNAME into the buffer
//			memcpy((void *) p, (void *) &qname[start], end - start);
//			p = &p[end - start];
//
//			// Updates the numbers, and continues or breaks
//			start = ++end;
//
//			if (*c == '\0') break;
//			else continue;
//		}
//
//		++end;
//	}

	// Adds the NULL at the end of the label
	*p = '\0';
	++p;

	// Sets the QType and QClass
	dns_question_t *question = (dns_question_t *) p;
	question->qclass = qclass;
	question->qtype = qtype;

	// Returns the pointer to next byte
	return question->next;
}

dns_label_seg_t *dns_label_parser_next(dns_label_seg_t *p)
{
	p = (dns_label_seg_t *) &p->next[p->len];
	if (p->len == '\0') return NULL;
	return p;
}
