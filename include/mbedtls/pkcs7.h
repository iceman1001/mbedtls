/**
 * \file pkcs7.h
 *
 * \brief PKCS7 generic defines and structures
 *  https://tools.ietf.org/html/rfc2315
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * Note: For the time being, this implementation of the PKCS7 cryptographic
 * message syntax is a partial implementation of RFC 2315.
 * Differences include:
 *  - The RFC specifies 6 different content types. The only type currently
 *    supported in Mbed TLS is the signed data content type.
 *  - The only supported PKCS7 Signed Data syntax version is version 1
 *  - The RFC specifies support for BER. This implementation is limited to
 *    DER only.
 *  - The RFC specifies that multiple digest algorithms can be specified
 *    in the Signed Data type. Only one digest algorithm is supported in Mbed TLS.
 *  - The RFC specifies the Signed Data type can contain multiple X509 or PKCS6
 *    certificates. In Mbed TLS, this list can only contain 0 or 1 certificates
 *    and they must be in X509 format.
 *  - The RFC specifies the Signed Data type can contain
 *    certificate-revocation lists (crls). This implementation has no support
 *    for crls so it is assumed to be an empty list.
 *  - The RFC allows for SignerInfo structure to optionally contain
 *    unauthenticatedAttributes and authenticatedAttributes. In Mbed TLS it is
 *    assumed these fields are empty.
 */

#ifndef MBEDTLS_PKCS7_H
#define MBEDTLS_PKCS7_H

#include "mbedtls/private_access.h"

#include "mbedtls/build_info.h"

#include "mbedtls/asn1.h"
#include "mbedtls/x509.h"
#include "mbedtls/x509_crt.h"

/**
 * \name PKCS7 Module Error codes
 * \{
 */
#define MBEDTLS_ERR_PKCS7_INVALID_FORMAT                   -0x5300  /**< The format is invalid, e.g. different type expected. */
#define MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE              -0x5380  /**< Unavailable feature, e.g. anything other than signed data. */
#define MBEDTLS_ERR_PKCS7_INVALID_VERSION                  -0x5400  /**< The PKCS7 version element is invalid or cannot be parsed. */
#define MBEDTLS_ERR_PKCS7_INVALID_CONTENT_INFO             -0x5480  /**< The PKCS7 content info invalid or cannot be parsed. */
#define MBEDTLS_ERR_PKCS7_INVALID_ALG                      -0x5500  /**< The algorithm tag or value is invalid or cannot be parsed. */
#define MBEDTLS_ERR_PKCS7_INVALID_CERT                     -0x5580  /**< The certificate tag or value is invalid or cannot be parsed. */
#define MBEDTLS_ERR_PKCS7_INVALID_SIGNATURE                -0x5600  /**< Error parsing the signature */
#define MBEDTLS_ERR_PKCS7_INVALID_SIGNER_INFO              -0x5680  /**< Error parsing the signer's info */
#define MBEDTLS_ERR_PKCS7_BAD_INPUT_DATA                   -0x5700  /**< Input invalid. */
#define MBEDTLS_ERR_PKCS7_ALLOC_FAILED                     -0x5780  /**< Allocation of memory failed. */
#define MBEDTLS_ERR_PKCS7_VERIFY_FAIL                      -0x5800  /**< Verification Failed */
#define MBEDTLS_ERR_PKCS7_CERT_DATE_INVALID                -0x5880  /**< The PKCS7 date issued/expired dates are invalid */
/* \} name */

/**
 * \name PKCS7 Supported Version
 * \{
 */
#define MBEDTLS_PKCS7_SUPPORTED_VERSION                           0x01
/* \} name */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef mbedtls_asn1_buf mbedtls_pkcs7_buf;

/**
 * Container for ASN1 named information objects.
 * It allows for Relative Distinguished Names (e.g. cn=localhost,ou=code,etc.).
 */
typedef mbedtls_asn1_named_data mbedtls_pkcs7_name;

/**
 * Container for a sequence of ASN.1 items
 */
typedef mbedtls_asn1_sequence mbedtls_pkcs7_sequence;

/**
 * PKCS7 types
 */
typedef enum {
    MBEDTLS_PKCS7_NONE=0,
    MBEDTLS_PKCS7_DATA,
    MBEDTLS_PKCS7_SIGNED_DATA,
    MBEDTLS_PKCS7_ENVELOPED_DATA,
    MBEDTLS_PKCS7_SIGNED_AND_ENVELOPED_DATA,
    MBEDTLS_PKCS7_DIGESTED_DATA,
    MBEDTLS_PKCS7_ENCRYPTED_DATA,
}
mbedtls_pkcs7_type;

/**
 * Structure holding PKCS7 signer info
 */
typedef struct mbedtls_pkcs7_signer_info
{
    int MBEDTLS_PRIVATE(version);
    mbedtls_x509_buf MBEDTLS_PRIVATE(serial);
    mbedtls_x509_name MBEDTLS_PRIVATE(issuer);
    mbedtls_x509_buf MBEDTLS_PRIVATE(issuer_raw);
    mbedtls_x509_buf MBEDTLS_PRIVATE(alg_identifier);
    mbedtls_x509_buf MBEDTLS_PRIVATE(sig_alg_identifier);
    mbedtls_x509_buf MBEDTLS_PRIVATE(sig);
    struct mbedtls_pkcs7_signer_info *MBEDTLS_PRIVATE(next);
}
mbedtls_pkcs7_signer_info;

/**
 * Structure holding attached data as part of PKCS7 signed data format
 */
typedef struct mbedtls_pkcs7_data
{
    mbedtls_pkcs7_buf MBEDTLS_PRIVATE(oid);
    mbedtls_pkcs7_buf MBEDTLS_PRIVATE(data);
}
mbedtls_pkcs7_data;

/**
 * Structure holding the signed data section
 */
typedef struct mbedtls_pkcs7_signed_data
{
    int MBEDTLS_PRIVATE(version);
    mbedtls_pkcs7_buf MBEDTLS_PRIVATE(digest_alg_identifiers);
    struct mbedtls_pkcs7_data MBEDTLS_PRIVATE(content);
    int MBEDTLS_PRIVATE(no_of_certs);
    mbedtls_x509_crt MBEDTLS_PRIVATE(certs);
    int MBEDTLS_PRIVATE(no_of_crls);
    mbedtls_x509_crl MBEDTLS_PRIVATE(crl);
    int MBEDTLS_PRIVATE(no_of_signers);
    mbedtls_pkcs7_signer_info MBEDTLS_PRIVATE(signers);
}
mbedtls_pkcs7_signed_data;

/**
 * Structure holding PKCS7 structure, only signed data for now
 */
typedef struct mbedtls_pkcs7
{
    mbedtls_pkcs7_buf MBEDTLS_PRIVATE(raw);
    mbedtls_pkcs7_buf MBEDTLS_PRIVATE(content_type_oid);
    mbedtls_pkcs7_signed_data MBEDTLS_PRIVATE(signed_data);
}
mbedtls_pkcs7;

/**
 * \brief          Initialize pkcs7 structure.
 *
 * \param pkcs7    pkcs7 structure.
 */
void mbedtls_pkcs7_init( mbedtls_pkcs7 *pkcs7 );

/**
 * \brief          Parse a single DER formatted pkcs7 content.
 *
 * \param pkcs7    The pkcs7 structure to be filled by parser for the output.
 * \param buf      The buffer holding the DER encoded pkcs7.
 * \param buflen   The size in Bytes of \p buf.
 *
 * \note           This function makes an internal copy of the PKCS7 buffer
 *                 \p buf. In particular, \p buf may be destroyed or reused
 *                 after this call returns.
 *
 * \return         The \c mbedtls_pkcs7_type of \p buf, if successful.
 * \return         A negative error code on failure.
 */
int mbedtls_pkcs7_parse_der( mbedtls_pkcs7 *pkcs7, const unsigned char *buf,
                             const size_t buflen );

/**
 * \brief          Verification of PKCS7 signature.
 *
 * \param pkcs7    PKCS7 structure containing signature.
 * \param cert     Certificate containing key to verify signature.
 * \param data     Plain data on which signature has to be verified.
 * \param datalen  Length of the data.
 *
 * \note           This function internally calculates the hash on the supplied
 *                 plain data for signature verification.
 *
 * \return         A negative error code on failure.
 */
int mbedtls_pkcs7_signed_data_verify( mbedtls_pkcs7 *pkcs7,
                                      const mbedtls_x509_crt *cert,
                                      const unsigned char *data,
                                      size_t datalen );

/**
 * \brief          Verification of PKCS7 signature.
 *
 * \param pkcs7    PKCS7 structure containing signature.
 * \param cert     Certificate containing key to verify signature.
 * \param hash     Hash of the plain data on which signature has to be verified.
 * \param hashlen  Length of the hash.
 *
 * \note           This function is different from mbedtls_pkcs7_signed_data_verify()
 *                 in a way that it directly recieves the hash of the data.
 *
 * \return         A negative error code on failure.
 */
int mbedtls_pkcs7_signed_hash_verify( mbedtls_pkcs7 *pkcs7,
                                      const mbedtls_x509_crt *cert,
                                      const unsigned char *hash, size_t hashlen);

/**
 * \brief          Unallocate all PKCS7 data and zeroize the memory.
 *                 It doesn't free pkcs7 itself. It should be done by the caller.
 *
 * \param pkcs7    PKCS7 structure to free.
 */
void mbedtls_pkcs7_free( mbedtls_pkcs7 *pkcs7 );

#ifdef __cplusplus
}
#endif

#endif /* pkcs7.h */
