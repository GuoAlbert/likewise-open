/*
 * Copyright (c) Likewise Software.  All rights Reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Module Name:
 *
 *        client.h
 *
 * Abstract:
 *
 *        Multi-threaded client API (public header)
 *
 * Authors: Brian Koropoff (bkoropoff@likewisesoftware.com)
 *
 */
#ifndef __LWMSG_CLIENT_H__
#define __LWMSG_CLIENT_H__

/**
 * @file client.h
 * @brief Multithreaded Client API
 */

#include <lwmsg/assoc.h>
#include <lwmsg/status.h>
#include <lwmsg/common.h>
#include <lwmsg/connection.h>

/**
 * @defgroup client Client
 * @ingroup public
 * @brief Multithreaded client abstraction
 *
 * This module provides a convenient, thread-safe API that
 * automatically manages concurrent connections to an LWMsg-based
 * server.
 *
 * This API is not available in the non-threaded LWMsg library
 * (lwmsg_nothr)
 */
/* @{ */

/**
 * @brief Client object
 *
 * An opaque structure which represents a single client.
 * A client object supports exchanging messages with
 * exactly one server endpoint within exactly one session.
 * Thus, connection state such as handles cannot be shared
 * between different client objects, but are safe to share
 * between associations from the same client object.
 *
 * Unlike most structures in LWMsg, clients are fully
 * thread-safe.
 */
typedef struct LWMsgClient LWMsgClient;

/**
 * @brief Create a new client object
 *
 * Creates a new client object with no endpoint set
 * and all options set to their default values.  Before
 * the new object may be used for messaging, the following
 * must first occur:
 *
 * <ul>
 * <li>A protocol must be established using one of
 * lwmsg_client_add_protocol_spec() or lwmsg_client_set_protocol()</li>
 * <li>An endpoint must be set with lwmsg_client_set_endpoint()</li>
 * </ul>
 *
 * @param out_client the created client object
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_new(
    LWMsgClient** out_client
    );

/**
 * @brief Set server endpoint
 *
 * Sets the server endpoint with which messages
 * will be exchanged.  This function may not be
 * called if an endpoint is already set.
 *
 * @param client the client object
 * @param mode the connection mode (local or remote)
 * @param endpoint the endpoint address
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_code{INVALID, the endpoint is invalid or an endpoint is already set}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_set_endpoint(
    LWMsgClient* client,
    LWMsgConnectionMode mode,
    const char* endpoint
    );

/**
 * @brief Add protocol specification
 *
 * Adds a protocol specification to the protocol
 * understood by the specified client object.  This
 * is a convenient shortcut that allows you to avoid
 * creating and populating a protocol object by hand.
 * This function may not be used in conjunction with
 * lwmsg_client_set_protocol().  This function may not
 * be called after the client is already in use.
 *
 * @param client the client object
 * @param spec the protocol specification
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_code{INVALID, a protocol has already been set or the client is already in use}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_add_protocol_spec(
    LWMsgClient* client,
    LWMsgProtocolSpec* spec
    );

/**
 * @brief Set protocol
 *
 * Sets the protocol object used by the specified
 * client object.  This function may not be called
 * after a protocol has already been set.  This function
 * may not be used in conjunction with lwmsg_client_add_protocol_spec()
 *
 * @warning Making further modifications to the specified
 * protocol object after calling this function will result
 * in undefined behavior.
 * 
 * @param client the client object
 * @param protocol the protocol object
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_code{INVALID, a protocol has already been set}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_set_protocol(
    LWMsgClient* client,
    LWMsgProtocol* protocol
    );

/**
 * @brief Acquire association for messaging
 *
 * Acquires an association suitable for exchanging messages
 * with the server.  The association is guaranteed to not
 * be in use by any other thread.  If multiple messages
 * are going to be exchanged in a row, acquiring an association
 * first is more efficient than multiple calls to lwmsg_client_send_transact().
 * If the maximum number of concurrent connections set by
 * lwmsg_client_set_max_concurrent() have already been acquired,
 * this function will block until an association becomes available.
 *
 * @param client the client object
 * @param out_assoc the acquired association
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_code{INVALID, the client has not been fully configured}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_acquire_assoc(
    LWMsgClient* client,
    LWMsgAssoc** out_assoc
    );

/**
 * @brief Release association
 *
 * Releases an association that was previously acquired by
 * lwmsg_client_acquire_assoc().
 *
 * @warning releasing an association that is in a non-ready state
 * has undefined behavior
 * @warning releasing an association that was not acquired by
 * lwmsg_client_acquire_assoc() has undefined behavior
 *
 * @param client the client object
 * @param assoc the association
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_release_assoc(
    LWMsgClient* client,
    LWMsgAssoc* assoc
    );

/**
 * @brief Create detached association
 *
 * Creates a fresh association suitable for
 * exchanging messages with the server that does not
 * count against the maximum number of concurrent connections
 * set with lwmsg_client_set_max_concurrent().  This association
 * does not need to be (and should not be) released with
 * lwmsg_client_release_assoc().  Unlike lwmsg_client_acquire_assoc(),
 * this function will never block.
 *
 * @param client the client object
 * @param out_assoc the created association
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_create_assoc(
    LWMsgClient* client,
    LWMsgAssoc** out_assoc
    );

/**
 * @brief Send a message
 *
 * Sends a message to the server.  This function implicitly
 * acquires and releases an association in order to send the
 * message and may block for the same reasons as
 * lwmsg_client_acquire_assoc().
 * 
 * @param client the client object
 * @param message the message to send
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_etc{this function may fail with the same errors as lwmsg_assoc_send_message()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_send_message(
    LWMsgClient* client,
    LWMsgMessage* message
    );

/**
 * @brief Receive a message
 *
 * Receives a message from the server.  This function implicitly
 * acquires and releases an association in order to receive the
 * message and may block for the same reasons as
 * lwmsg_client_acquire_assoc().
 * 
 * @param client the client object
 * @param message where the received message will be stored
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_etc{this function may fail with the same errors as lwmsg_assoc_recv_message()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_recv_message(
    LWMsgClient* client,
    LWMsgMessage* message
    );

/**
 * @brief Send a message and receive a reply
 *
 * Sends a message to the server and receives a reply.
 * This function implicitly acquires and releases an
 * association and may block for the same reasons as
 * lwmsg_client_acquire_assoc().
 * 
 * @param client the client object
 * @param send_msg the message to send
 * @param recv_msg where the reply will be stored
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_etc{this function may fail with the same errors as lwmsg_assoc_transact_message()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_send_message_transact(
    LWMsgClient* client,
    LWMsgMessage* send_msg,
    LWMsgMessage* recv_msg
    );

/**
 * @brief Send a message (simple)
 *
 * Sends a message to the server.  This function is equivalent
 * to lwmsg_client_send_message() but does not require use
 * of a full LWMsgMessage structure.
 * 
 * @param client the client object
 * @param type the type of the message to send
 * @param object the payload of the message to send
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_etc{this function may fail with the same errors as lwmsg_assoc_send()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_send(
    LWMsgClient* client,
    LWMsgMessageTag type,
    void* object
    );

/**
 * @brief Receive a message (simple)
 *
 * Receives a message from the server.  This function is equivalent
 * to lwmsg_client_recv_message() but does not require use
 * of a full LWMsgMessage structure.
 * 
 * @param client the client object
 * @param out_type the type of the received message
 * @param out_object the payload of the received to message
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_etc{this function may fail with the same errors as lwmsg_assoc_recv()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_recv(
    LWMsgClient* client,
    LWMsgMessageTag* out_type,
    void** out_object
    );

/**
 * @brief Send a message and receive a reply (simple).
 *
 * Sends a message to the server and receives a reply.
 * This function is similar to lwmsg_client_send_message_transact(),
 * but does not require the use of full LWMsgMessage structures.
 * 
 * @param client the client object
 * @param in_type the type of the message to send
 * @param in_object the payload of the message to send
 * @param out_type the type of the received reply
 * @param out_object the payload the the received reply
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_etc{this function may fail with the same errors as lwmsg_assoc_transact()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_send_transact(
    LWMsgClient* client,
    LWMsgMessageTag in_type,
    void* in_object,
    LWMsgMessageTag* out_type,
    void** out_object
    );

/**
 * @brief Set maximum concurrent connections
 *
 * Sets the maximum number of concurrent connections
 * which the specified client object will make to the server.
 * This function may be called even when the client is in active
 * use.  An increase in the maximum will immediately allow additional
 * connections to be made.  A decrease will cause excess idle connections
 * to be closed and excess connections that are in use to be closed
 * when they become idle.
 *
 * @param client the client object
 * @param max the maximum number of concurrent connections
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_memory
 * @lwmsg_etc{this function may fail for the same reasons as lwmsg_assoc_close()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_set_max_concurrent(
    LWMsgClient* client,
    size_t max
    );

/**
 * @brief Cleanly shut down client
 *
 * Cleanly shuts down the client, including waiting for all active connections
 * to become inactive.
 *
 * @param client the client object
 * @lwmsg_status
 * @lwmsg_success
 * @lwmsg_etc{thus function may fail for the same reasons as lwmsg_assoc_close()}
 * @lwmsg_endstatus
 */
LWMsgStatus
lwmsg_client_shutdown(
    LWMsgClient* client
    );

/**
 * @brief Delete a client object
 *
 * Deletes a client object, freeing all resources associated with it.  Unlike
 * lwmsg_client_shutdown(), this function will aggressively terminate any
 * remaining active connections.
 *
 * @param client the client object
 */
void
lwmsg_client_delete(
    LWMsgClient* client
    );
    
/* @} */

#endif
