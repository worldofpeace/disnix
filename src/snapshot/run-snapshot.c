/*
 * Disnix - A Nix-based distributed service deployment tool
 * Copyright (C) 2008-2018  Sander van der Burg
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "run-snapshot.h"
#include <manifest.h>
#include <snapshotmapping.h>

int run_snapshot(const gchar *manifest_file, const unsigned int max_concurrent_transfers, const unsigned int flags, const int keep, const gchar *old_manifest, const gchar *coordinator_profile_path, gchar *profile, const gchar *container_filter, const gchar *component_filter)
{
    /* Generate a distribution array from the manifest file */
    Manifest *manifest = open_provided_or_previous_manifest_file(manifest_file, coordinator_profile_path, profile, MANIFEST_SNAPSHOT_FLAG, container_filter, component_filter);

    if(manifest == NULL)
    {
        g_print("[coordinator]: Error while opening manifest file!\n");
        return 1;
    }
    else
    {
        if(manifest_file == NULL) /* When no manifest file is provided as a parameter -> always snapshot the entrie environment */
            return !snapshot(manifest, NULL, max_concurrent_transfers, flags | FLAG_NO_UPGRADE, keep);
        else
        {
            GPtrArray *old_snapshots_array = NULL;
            gchar *old_manifest_file;
            int exit_status;

            /* If no old manifest is provided, try to open previous manifest from profile */
            if(old_manifest == NULL)
                old_manifest_file = determine_previous_manifest_file(coordinator_profile_path, profile);
            else
                old_manifest_file = g_strdup(old_manifest);

            g_printerr("[coordinator]: Using previous manifest: %s\n", old_manifest_file);

            /* Take the snapshots from the previous manifest, if appropriate */
            if(!(flags & FLAG_NO_UPGRADE) || old_manifest_file != NULL)
                old_snapshots_array = create_snapshots_array(old_manifest_file, container_filter, component_filter);

            /* Take snapshots and transfer them */
            exit_status = !snapshot(manifest, old_snapshots_array, max_concurrent_transfers, flags, keep);

            /* Cleanup */
            if(!(flags & FLAG_NO_UPGRADE) || old_manifest_file != NULL)
                delete_snapshots_array(old_snapshots_array);

            delete_manifest(manifest);

            /* Return exit status */
            return exit_status;
        }
    }
}