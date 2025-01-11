#include <kos.h>
#include <dc/pvr.h>
#include "pvrtex.h"

static dttex_info_t texture;

static void init_poly_context(pvr_poly_cxt_t *cxt) {
    pvr_poly_cxt_txr(cxt, PVR_LIST_OP_POLY, texture.pvrformat, texture.width,
                     texture.height, texture.ptr, PVR_FILTER_BILINEAR);
}

static void render_quad(void) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t *vert;
    pvr_dr_state_t dr_state;

    float center_x = 640.0f / 2;
    float center_y = 480.0f / 2;
    float size = 240.0f;   
    
    init_poly_context(&cxt);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    pvr_dr_init(&dr_state);

    vert = pvr_dr_target(dr_state);
    vert->flags = PVR_CMD_VERTEX;
    vert->x = center_x - size/2; vert->y = center_y - size/2; vert->z = 1.0f;
    vert->u = 0.0f;
    vert->v = 0.0f;
     pvr_dr_commit(vert);

    vert = pvr_dr_target(dr_state);
    vert->flags = PVR_CMD_VERTEX;
    vert->x = center_x + size/2; vert->y = center_y - size/2; vert->z = 1.0f;
    vert->u = 1.0f;
    vert->v = 0.0f;
     pvr_dr_commit(vert);

    vert = pvr_dr_target(dr_state);
    vert->flags = PVR_CMD_VERTEX;
    vert->x = center_x - size/2; vert->y = center_y + size/2; vert->z = 1.0f;
    vert->u = 0.0f;
    vert->v = 1.0f;
     pvr_dr_commit(vert);

    vert = pvr_dr_target(dr_state);
    vert->flags = PVR_CMD_VERTEX_EOL;
    vert->x = center_x + size/2; vert->y = center_y + size/2; vert->z = 1.0f;
    vert->u = 1.0f;
    vert->v = 1.0f;
     pvr_dr_commit(vert);

    pvr_dr_finish();
}

int main(int argc, char **argv) {
    pvr_init_params_t params = {
        { PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },
        512 * 1024,
        0,
        0,
        0
    };
    maple_device_t *cont;
    cont_state_t *state;

    pvr_init(&params);
    pvr_set_bg_color(0.0f, 0.0f, 0.0f);

    if (!pvrtex_load("/rd/dc.dt", &texture)) {
        printf("Failed to load texture.\n");
        return -1;
    }

    while (1) {
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

        pvr_wait_ready();
        pvr_scene_begin();
        pvr_list_begin(PVR_LIST_OP_POLY);

        render_quad();

        pvr_list_finish();
        pvr_scene_finish();

          if(cont) {
            state = (cont_state_t *)maple_dev_status(cont);

          
            if(state->buttons & CONT_START){
                break;
            }
            }
        }

    pvrtex_unload(&texture);
    pvr_shutdown();

    return 0;
}
