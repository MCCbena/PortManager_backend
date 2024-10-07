#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "DatabaseUtil.h"

#define node "oracle1"
#define NIC "ens3"

struct PortInfo{
    char* ipaddress;
    int port;
    char* protocol;
};

struct PortInfo makePortInfo();
void destroy_port_info(struct PortInfo*);
int port_info_equal(struct PortInfo, struct PortInfo);
int refresh(MYSQL*);
int byteSize(const char*);

struct PortInfo *port_infos;
int size = 0;

int main(void) {
    MYSQL *conn = getConnection();
    port_infos = calloc(24, 128);
    while (1) {
        if(conn!=NULL) {
            refresh(conn);
            sleep(1);
        } else{
            printf("接続エラー\n");
            return 0;
        }
    }
}

int refresh(MYSQL *conn){
    struct PortInfo *latest_port_info = calloc(24, 128);
    int latest_port_array_size = 0;
    MYSQL_ROW row;
    /*
     * 1=コマンド実行時のエラー
     */
    int error=0;

    //データベースからデータを取得
    char* command = calloc(256, 1);
    sprintf(command, "SELECT * FROM %s", node);
    struct Response_sql responseSql = sendCommandHasResponse(conn, command);
    //情報の格納
    if(conn!=NULL) {
        if (responseSql.error != 1) {
            while ((row = mysql_fetch_row(responseSql.response)) != NULL) {
                //構造体の作成
                struct PortInfo temp_data = makePortInfo();
                //代入
                memcpy(temp_data.ipaddress, row[1], byteSize(row[1]));
                temp_data.port = atoi(row[2]);
                memcpy(temp_data.protocol, row[3], byteSize(row[3]));
                //配列に代入
                latest_port_info[latest_port_array_size] = temp_data;
                latest_port_array_size++;
            }
        } else error = 2;
    } else error = 1;
    free(command);
    mysql_free_result(responseSql.response);


    //データの照合
    for(int i0 = 0; i0 < size; i0++){//削除されたルールをチェック
        char is_equal = 0;
        if(port_infos[i0].ipaddress == NULL) break;
        for(int i1 = 0; i1 < latest_port_array_size; i1++){
            if (!port_info_equal(port_infos[i0], latest_port_info[i1])) {
                is_equal = 1;
                break;
            }
        }
        if(!is_equal) {
            printf("削除:IP:%s, ポート:%d, プロトコル:%s\n", port_infos[i0].ipaddress, port_infos[i0].port,
                   port_infos[i0].protocol);
            char *ip_command = calloc(256, 1);
            sprintf(ip_command,
                    "iptables -t nat -D PREROUTING -i %s -p %s --dport %d -j DNAT --to-destination %s;",
                    NIC, port_infos[i0].protocol, port_infos[i0].port, port_infos[i0].ipaddress);
            if(system(ip_command)==-1) printf("コマンドの実行に失敗しました。%s\n", ip_command);
            free(ip_command);
            //配列からポートを削除してfree
            destroy_port_info(&port_infos[i0]);
            port_infos[i0].ipaddress = NULL;
            if(i0+1==size) {
                printf("サイズ縮小。\n");
                size--;
            }
        }
    }

    for(int i0 = 0; i0 < latest_port_array_size; i0++) {//作成されたルールをチェック
        char is_equal = 0;
        for (int i1 = 0; i1 < size; i1++) {
            if(port_infos[i1].ipaddress != NULL) {
                if (!port_info_equal(port_infos[i1], latest_port_info[i0])) {
                    is_equal = 1;
                    break;
                }
            }
        }
        if (!is_equal) {
            printf("作成:IP:%s, ポート:%d, プロトコル:%s\n", latest_port_info[i0].ipaddress, latest_port_info[i0].port,
                   latest_port_info[i0].protocol);

            char *ip_command = calloc(256, 1);
            sprintf(ip_command,
                    "iptables -t nat -A PREROUTING -i %s -p %s --dport %d -j DNAT --to-destination %s;",
                    NIC, latest_port_info[i0].protocol, latest_port_info[i0].port, latest_port_info[i0].ipaddress);
            if(system(ip_command)==-1) printf("コマンドの実行に失敗しました。%s\n", ip_command);
            free(ip_command);
            //配列の空いている部分を見つけて挿入
            int input_array_index = -1;
            for (int i1 = 0; i1 < size; i1++) {
                if (port_infos[i1].ipaddress == NULL) {
                    input_array_index = i1;
                    printf("インデックス: %d データ挿入\n", input_array_index);
                    break;
                }
            }

            //空いているポートがなければ新規作成
            if (input_array_index == -1) {
                printf("配列空き容量なし。新作成。\n");
                input_array_index = size++;
            }

            port_infos[input_array_index] = makePortInfo();
            memcpy(port_infos[input_array_index].ipaddress, latest_port_info[i0].ipaddress, 256);
            port_infos[input_array_index].port = latest_port_info[i0].port;
            memcpy(port_infos[input_array_index].protocol, latest_port_info[i0].protocol, 16);
        }
    }

    for(int i = 0; i < latest_port_array_size; i++){
        destroy_port_info(&latest_port_info[i]);
    }
    free(latest_port_info);

    return error;
}

struct PortInfo makePortInfo(){
    struct PortInfo port_info;

    port_info.ipaddress = calloc(256, 1);
    port_info.protocol = calloc(16, 1);

    return port_info;
}

int byteSize(const char* data){
    int i = 0;
    while (data[i]!=0){
        i++;
    }

    return i;
}

/*
 * イコールであるときは0、そうでないときは1を返します。
 */
int port_info_equal(struct PortInfo portInfo0, struct PortInfo portInfo1){
    if(strcmp(portInfo0.ipaddress, portInfo1.ipaddress) == 0 &&
    portInfo0.port == portInfo1.port &&
            strcmp(portInfo0.protocol, portInfo1.protocol) == 0){
        return 0;
    }
    return 1;
}

void destroy_port_info(struct PortInfo* portInfo){
    free(portInfo->ipaddress);
    free(portInfo->protocol);
}