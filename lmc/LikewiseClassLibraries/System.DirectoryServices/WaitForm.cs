/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
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

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace System.DirectoryServices
{
    /// <summary>
    ///
    /// </summary>
    public partial class WaitForm : Form
    {
        #region Data
        private BackgroundWorker backgroundWorker = null;
        private Timer timer = null;
        public bool bIsInterrupted = false;
        private DirectoryEntry de = null;
        #endregion

        /// <summary>
        ///
        /// </summary>
        public WaitForm(BackgroundWorker backgroundWorker,Timer timer, DirectoryEntry de)
        {
            InitializeComponent();
            this.backgroundWorker = backgroundWorker;
            this.timer = timer;

            //Assign to use when the thread got interrupted in the middle
            this.de = de;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            bIsInterrupted = true;
            Close();
        }

        /// <summary>
        ///
        /// </summary>
        public ProgressBar pb
        {
            get
            {
                return progressBar;
            }
        }

        /// <summary>
        ///
        /// </summary>
        public Label labelStatus
        {
            get
            {
                return lbStatus;
            }

        }

        private void WaitForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing && backgroundWorker.IsBusy)
            {
                timer.Stop();
                bIsInterrupted = true;

                if (de != null)
                {
                    de.DirContext.Ldap_CancelSynchronous();
                }

                backgroundWorker.ReportProgress(0);
                //backgroundWorker.CancelAsync();
            }
        }
    }
}