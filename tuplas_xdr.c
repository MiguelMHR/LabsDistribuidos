/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "tuplas.h"

bool_t
xdr_message (XDR *xdrs, message *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->key_or_return))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->value1, 256,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->n_value2))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->v_value2, 32,
		sizeof (double), (xdrproc_t) xdr_double))
		 return FALSE;
	return TRUE;
}
