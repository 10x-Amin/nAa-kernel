/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora Forum nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * Alternatively, provided that this notice is retained in full, this software
 * may be relicensed by the recipient under the terms of the GNU General Public
 * License version 2 ("GPL") and only version 2, in which case the provisions of
 * the GPL apply INSTEAD OF those given above.  If the recipient relicenses the
 * software under the GPL, then the identification text in the MODULE_LICENSE
 * macro must be changed to reflect "GPLv2" instead of "Dual BSD/GPL".  Once a
 * recipient changes the license terms to the GPL, subsequent recipients shall
 * not relicense under alternate licensing terms, including the BSD or dual
 * BSD/GPL terms.  In addition, the following license statement immediately
 * below and between the words START and END shall also then apply when this
 * software is relicensed under the GPL:
 *
 * START
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 and only version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * END
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <linux/err.h>
#include <mach/rpc_hsusb.h>
#include <mach/msm_hsusb.h>
#include <mach/msm_rpcrouter.h>
#include <mach/board.h>

#define PM_APP_OTG_PROG		0x30000080
#define PM_APP_OTG_VERS		0x00010001

#define PM_APP_OTG_INIT_PHY			17
#define PM_APP_OTG_RESET_PHY			18
#define PM_APP_OTG_SUSPEND_PHY			7
#define PM_APP_OTG_RESUME_PHY			8
#define PM_APP_OTG_DEV_DISCONNECTED		9
#define PM_APP_OTG_SET_WAKEUP			10
#define PM_APP_OTG_ACQUIRE_BUS			3
#define PM_APP_OTG_RELINQUISH_BUS		4

#define PM_APP_OTG_INIT_DONE_CB_PROC		1
#define PM_APP_OTG_HOST_INIT_CB_PROC		3
#define PM_APP_OTG_REMOTE_DEV_LOST_CB_PROC	8
#define PM_APP_OTG_REMOTE_DEV_RESUMED_CB_PROC	9
#define PM_APP_OTG_ERROR_NOTIFY_CB_PROC		11

#define NUM_OF_CALLBACKS			11
static struct msm_rpc_client *client;
static struct msm_otg_ops *host_ops;

static int msm_fsusb_rpc_arg(struct msm_rpc_client *client,
			     void *buf, void *data)
{
	int i, size = 0;
	uint32_t proc = *(uint32_t *)data;

	switch (proc) {
	case PM_APP_OTG_INIT_PHY: {
		for (i = 0; i < NUM_OF_CALLBACKS; i++) {
			*((uint32_t *)buf) = cpu_to_be32(0x11111111);
			size += sizeof(uint32_t);
			buf += sizeof(uint32_t);
		}

		/* sleep_assert callback fucntion will be registered locally*/
		*((uint32_t *)buf) = cpu_to_be32(0xffffffff);
		size += sizeof(uint32_t);
		break;
	}
	case PM_APP_OTG_SET_WAKEUP: {
			*((uint32_t *)buf) = cpu_to_be32(1);
			size += sizeof(uint32_t);
			break;
	}
	case PM_APP_OTG_ACQUIRE_BUS: {
			*((uint32_t *)buf) = cpu_to_be32(0xffffffff);
			size += sizeof(uint32_t);
			break;
	}
	default:
		pr_info("%s: No arguments expected\n", __func__);
	}
	return size;
}

int msm_fsusb_init_phy(void)
{
	uint32_t data = PM_APP_OTG_INIT_PHY;

	return msm_rpc_client_req(client,
			PM_APP_OTG_INIT_PHY,
			msm_fsusb_rpc_arg, &data,
			NULL, NULL, -1);
}
EXPORT_SYMBOL(msm_fsusb_init_phy);

int msm_fsusb_reset_phy(void)
{
	return msm_rpc_client_req(client,
			PM_APP_OTG_RESET_PHY,
			NULL, NULL,
			NULL, NULL, -1);

}
EXPORT_SYMBOL(msm_fsusb_reset_phy);

int msm_fsusb_suspend_phy(void)
{
	return msm_rpc_client_req(client,
			PM_APP_OTG_SUSPEND_PHY,
			NULL, NULL,
			NULL, NULL, -1);

}
EXPORT_SYMBOL(msm_fsusb_suspend_phy);

int msm_fsusb_resume_phy(void)
{
	return msm_rpc_client_req(client,
			PM_APP_OTG_RESUME_PHY,
			NULL, NULL,
			NULL, NULL, -1);

}
EXPORT_SYMBOL(msm_fsusb_resume_phy);

int msm_fsusb_remote_dev_disconnected(void)
{
	return msm_rpc_client_req(client,
			PM_APP_OTG_DEV_DISCONNECTED,
			NULL, NULL,
			NULL, NULL, -1);

}
EXPORT_SYMBOL(msm_fsusb_remote_dev_disconnected);

int  msm_fsusb_set_remote_wakeup(void)
{
	uint32_t data = PM_APP_OTG_SET_WAKEUP;

	return msm_rpc_client_req(client,
			PM_APP_OTG_SET_WAKEUP,
			msm_fsusb_rpc_arg, &data,
			NULL, NULL, -1);

}
EXPORT_SYMBOL(msm_fsusb_set_remote_wakeup);

static int msm_fsusb_acquire_bus(void)
{
	uint32_t data = PM_APP_OTG_ACQUIRE_BUS;

	return msm_rpc_client_req(client,
			PM_APP_OTG_ACQUIRE_BUS,
			msm_fsusb_rpc_arg, &data,
			NULL, NULL, -1);

}

static int msm_fsusb_relinquish_bus(void)
{
	return msm_rpc_client_req(client,
			PM_APP_OTG_RELINQUISH_BUS,
			NULL, NULL,
			NULL, NULL, -1);

}

static void msm_fsusb_request_session(void)
{
	int ret;

	ret = msm_fsusb_relinquish_bus();
	if (ret < 0)
		pr_err("relinquish_bus rpc failed\n");
	ret = msm_fsusb_acquire_bus();
	if (ret < 0)
		pr_err("acquire_bus rpc failed\n");
}

static int msm_fsusb_cb_func(struct msm_rpc_client *client,
			    void *buffer, int in_size)
{
	struct rpc_request_hdr *req;
	int rc;

	req = buffer;

	msm_rpc_start_accepted_reply(client, be32_to_cpu(req->xid),
				     RPC_ACCEPTSTAT_SUCCESS);
	rc = msm_rpc_send_accepted_reply(client, 0);
	if (rc) {
		pr_err("%s: sending reply failed: %d\n", __func__, rc);
		return rc;
	}

	switch (be32_to_cpu(req->procedure)) {
	case PM_APP_OTG_INIT_DONE_CB_PROC: {
		pr_debug("pm_app_otg_init_done callback received");
		msm_fsusb_request_session();
		break;
	}
	case PM_APP_OTG_HOST_INIT_CB_PROC: {
		pr_debug("pm_app_otg_host_init_cb_proc callback received");
		host_ops->request(host_ops->handle, REQUEST_START);
		break;
	}
	case PM_APP_OTG_REMOTE_DEV_LOST_CB_PROC: {
		pr_debug("pm_app_otg_remote_dev_lost_cb_proc"
				" callback received");
		msm_fsusb_acquire_bus();
		host_ops->request(host_ops->handle, REQUEST_STOP);
		break;
	}
	case PM_APP_OTG_REMOTE_DEV_RESUMED_CB_PROC: {
		pr_debug("pm_app_otg_remote_dev_resumed_cb_proc"
				"callback received");
		host_ops->request(host_ops->handle, REQUEST_RESUME);
		break;
	}
	case PM_APP_OTG_ERROR_NOTIFY_CB_PROC: {
		pr_err("pm_app_otg_error_notify_cb_proc callback received");
		break;
	}
	default:
		pr_err("%s: unknown callback(proc = %d) received\n",
					__func__, req->procedure);
	}
	return 0;
}

int msm_fsusb_rpc_init(struct msm_otg_ops *ops)
{
	host_ops = ops;
	client = msm_rpc_register_client("fsusb",
			PM_APP_OTG_PROG,
			PM_APP_OTG_VERS, 1,
			msm_fsusb_cb_func);
	if (IS_ERR(client)) {
		pr_err("%s: couldn't open rpc client\n", __func__);
		return PTR_ERR(client);
	}

	return 0;

}
EXPORT_SYMBOL(msm_fsusb_rpc_init);

void msm_fsusb_rpc_deinit(void)
{
	msm_rpc_unregister_client(client);
}
EXPORT_SYMBOL(msm_fsusb_rpc_deinit);
