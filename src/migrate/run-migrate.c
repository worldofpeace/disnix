#include "run-migrate.h"
#include <migrate.h>
#include <manifest.h>
#include <snapshotmapping.h>

int run_migrate(const gchar *manifest_file, const unsigned int max_concurrent_transfers, const unsigned int flags, const int keep, const gchar *old_manifest, const gchar *coordinator_profile_path, gchar *profile, const gchar *container_filter, const gchar *component_filter)
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
        GPtrArray *old_snapshots_array = NULL;
        gchar *old_manifest_file;
        int exit_status;

        if(old_manifest == NULL)
            old_manifest_file = determine_previous_manifest_file(coordinator_profile_path, profile);
        else
            old_manifest_file = g_strdup(old_manifest);

        if(!(flags & FLAG_NO_UPGRADE) && old_manifest != NULL)
            old_snapshots_array = create_snapshots_array(old_manifest_file, container_filter, component_filter);

        exit_status = !migrate(manifest, old_snapshots_array, max_concurrent_transfers, flags, keep);

        /* Cleanup */
        g_free(old_manifest_file);
        delete_snapshots_array(old_snapshots_array);
        delete_manifest(manifest);

        /* Return the exit status */
        return exit_status;
    }
}