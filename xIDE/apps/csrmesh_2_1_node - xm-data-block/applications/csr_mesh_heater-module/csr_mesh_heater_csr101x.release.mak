###########################################################
# Makefile generated by xIDE for uEnergy                   
#                                                          
# Project: csr_mesh_heater_csr101x
# Configuration: Release
# Generated: ���� ���� 1 14:54:45 2017
#                                                          
# WARNING: Do not edit this file. Any changes will be lost 
#          when the project is rebuilt.                    
#                                                          
###########################################################

XIDE_PROJECT=csr_mesh_heater_csr101x
XIDE_CONFIG=Release
OUTPUT=csr_mesh_heater_csr101x
OUTDIR=E:/CSR_uEnergy_SDK-2.6.1.7-data-block-8-14/apps/csrmesh_2_1_node - xm-data-block/applications/csr_mesh_heater-module
DEFS=-DSERVER -DPERIPHERAL -DOBSERVER -DTHIN_CM4_MESH_NODE -DENABLE_MESH_OTA 

OUTPUT_TYPE=0
USE_FLASH=1
ERASE_NVM=1
CSFILE_CSR101x_A05=csr_mesh_heater_csr101x_A05.keyr
MASTER_DB=app_gatt_db.db
LIBPATHS=..\mesh_common\mesh\libraries\csr_101x
INCPATHS=..\mesh_common\...
STRIP_SYMBOLS=0
FLASH_CLOCK=9
FLASH_MISO=11
OTAU_BOOTLOADER=1
OTAU_CSFILE=bootloader.keyr
OTAU_NAME=CSRmesh-OTA
OTAU_SECRET=00112233445566778899aabbccddeeff
OTAU_VERSION=7
OTAU_SLOT_1=0x11000
OTAU_SLOT_2=0x22000
OTAU_SLOT_END=0x40000

LIBS=csrmesh sensor_server ping_server attention_server time_client time_server action_server data_server data_client battery_server sensor_client largeobjecttransfer_server 
DBS=\
\
      ../mesh_common/server/gap/gap_service_db.db\
      ../mesh_common/server/gatt/gatt_service_db.db\
      ../mesh_common/server/mesh_control/mesh_control_service_db.db\
      ../mesh_common/server/csr_ota/csr_ota_db.db\
      app_gatt_db.db

INPUTS=\
      ../mesh_common/peripheral/conn_param_update.c\
      ../mesh_common/components/nvm_manager/nvm_access.c\
      ../mesh_common/components/connection_manager/cm_common.c\
      ../mesh_common/components/connection_manager/cm_hal.c\
      ../mesh_common/components/connection_manager/cm_private.c\
      ../mesh_common/components/connection_manager/cm_security.c\
      ../mesh_common/components/connection_manager/cm_server.c\
      ../mesh_common/components/connection_manager/cm_observer.c\
      ../mesh_common/components/connection_manager/cm_peripheral.c\
      ../mesh_common/mesh/drivers/battery_hw.c\
      ../mesh_common/mesh/drivers/iot_hw.c\
      ../mesh_common/mesh/drivers/csr_mesh_ps_ifce.c\
      ../mesh_common/mesh/handlers/attention_model/attention_model_handler.c\
      ../mesh_common/mesh/handlers/battery_model/battery_model_handler.c\
      ../mesh_common/mesh/handlers/core_mesh/core_mesh_handler.c\
      ../mesh_common/mesh/handlers/data_model/data_model_handler.c\
      ../mesh_common/mesh/handlers/sensor_model/sensor_model_handler.c\
      ../mesh_common/mesh/handlers/common/app_util.c\
      ../mesh_common/mesh/handlers/connection/connection_handler.c\
      ../mesh_common/mesh/handlers/advertisement/advertisement_handler.c\
      ../mesh_common/mesh/handlers/time_model/time_model_handler.c\
      ../mesh_common/mesh/handlers/action_model/action_model_handler.c\
      ../mesh_common/mesh/handlers/largeobjecttransfer_model/largeobjecttransfer_model_handler.c\
      ../mesh_common/server/gap/gap_service.c\
      ../mesh_common/server/gatt/gatt_service.c\
      ../mesh_common/server/mesh_control/mesh_control_service.c\
      ../mesh_common/server/csr_ota/csr_ota_service.c\
      main_app.c\
      app_mesh_handler.c\
      app_mesh_model_handler.c\
      byte_queue.c\
      label.c\
      time_uart.c\
      uart_time.c\
      Flash_Run.c\
      $(DBS)

KEYR=\
      csr_mesh_heater_csr101x_A05.keyr\
      bootloader.keyr


-include csr_mesh_heater_csr101x.mak
include $(SDK)/genmakefile.uenergy