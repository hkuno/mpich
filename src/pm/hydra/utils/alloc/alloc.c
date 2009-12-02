/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2008 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "hydra_utils.h"

void HYDU_init_user_global(struct HYD_user_global *user_global)
{
    user_global->bootstrap = NULL;
    user_global->bootstrap_exec = NULL;

    user_global->binding = NULL;
    user_global->bindlib = NULL;

    user_global->ckpointlib = NULL;
    user_global->ckpoint_prefix = NULL;
    user_global->ckpoint_restart = 0;

    user_global->enablex = -1;
    user_global->debug = -1;
    user_global->wdir = NULL;
    user_global->launch_mode = NULL;

    HYDU_init_global_env(&user_global->global_env);
}

void HYDU_init_global_env(struct HYD_env_global *global_env)
{
    global_env->system = NULL;
    global_env->user = NULL;
    global_env->inherited = NULL;
    global_env->prop = NULL;
}

static void init_node(struct HYD_node *node)
{
    node->hostname = NULL;
    node->core_count = 0;
    node->next = NULL;
}

HYD_status HYDU_alloc_node(struct HYD_node **node)
{
    HYD_status status = HYD_SUCCESS;

    HYDU_FUNC_ENTER();

    HYDU_MALLOC(*node, struct HYD_node *, sizeof(struct HYD_node), status);
    init_node(*node);

  fn_exit:
    HYDU_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

void HYDU_free_node_list(struct HYD_node *node_list)
{
    struct HYD_node *node, *tnode;

    node = node_list;
    while (node) {
        tnode = node->next;

        if (node->hostname)
            HYDU_FREE(node->hostname);
        HYDU_FREE(node);

        node = tnode;
    }
}

void HYDU_init_pg(struct HYD_pg *pg)
{
    pg->pgid = -1;
    pg->proxy_list = NULL;
    pg->pg_process_count = 0;
    pg->next = NULL;
}

HYD_status HYDU_alloc_pg(struct HYD_pg **pg)
{
    HYD_status status = HYD_SUCCESS;

    HYDU_FUNC_ENTER();

    HYDU_MALLOC(*pg, struct HYD_pg *, sizeof(struct HYD_pg), status);
    HYDU_init_pg(*pg);

  fn_exit:
    HYDU_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

void HYDU_free_pg_list(struct HYD_pg *pg_list)
{
    struct HYD_pg *pg, *tpg;

    pg = pg_list;
    while (pg) {
        tpg = pg->next;

        if (pg->proxy_list)
            HYDU_free_proxy_list(pg->proxy_list);
        HYDU_FREE(pg);

        pg = tpg;
    }
}

HYD_status HYDU_alloc_proxy(struct HYD_proxy **proxy)
{
    HYD_status status = HYD_SUCCESS;

    HYDU_FUNC_ENTER();

    HYDU_MALLOC(*proxy, struct HYD_proxy *, sizeof(struct HYD_proxy), status);

    init_node(&(*proxy)->node);

    (*proxy)->pid = -1;
    (*proxy)->in = -1;
    (*proxy)->out = -1;
    (*proxy)->err = -1;

    (*proxy)->proxy_id = -1;
    (*proxy)->exec_launch_info = NULL;

    (*proxy)->start_pid = -1;
    (*proxy)->proxy_process_count = 0;

    (*proxy)->exit_status = NULL;
    (*proxy)->control_fd = -1;

    (*proxy)->exec_list = NULL;
    (*proxy)->next = NULL;

  fn_exit:
    HYDU_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}


void HYDU_free_proxy_list(struct HYD_proxy *proxy_list)
{
    struct HYD_proxy *proxy, *tproxy;
    struct HYD_proxy_exec *exec, *texec;

    HYDU_FUNC_ENTER();

    proxy = proxy_list;
    while (proxy) {
        tproxy = proxy->next;

        if (proxy->node.hostname)
            HYDU_FREE(proxy->node.hostname);

        if (proxy->exec_launch_info) {
            HYDU_free_strlist(proxy->exec_launch_info);
            HYDU_FREE(proxy->exec_launch_info);
        }

        if (proxy->exit_status)
            HYDU_FREE(proxy->exit_status);

        exec = proxy->exec_list;
        while (exec) {
            texec = exec->next;
            HYDU_free_strlist(exec->exec);
            if (exec->user_env)
                HYDU_env_free(exec->user_env);
            if (exec->env_prop)
                HYDU_FREE(exec->env_prop);
            HYDU_FREE(exec);
            exec = texec;
        }

        HYDU_FREE(proxy);
        proxy = tproxy;
    }

    HYDU_FUNC_EXIT();
}


HYD_status HYDU_alloc_proxy_exec(struct HYD_proxy_exec **exec)
{
    HYD_status status = HYD_SUCCESS;

    HYDU_FUNC_ENTER();

    HYDU_MALLOC(*exec, struct HYD_proxy_exec *, sizeof(struct HYD_proxy_exec), status);
    (*exec)->exec[0] = NULL;
    (*exec)->proc_count = 0;
    (*exec)->env_prop = NULL;
    (*exec)->user_env = NULL;
    (*exec)->next = NULL;

  fn_exit:
    HYDU_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
