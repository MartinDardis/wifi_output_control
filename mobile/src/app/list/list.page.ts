import {Component, OnInit} from '@angular/core';
import {AlertController} from '@ionic/angular';
import {ConnectionService} from '../Services/connection.service';
import {Subscription} from 'rxjs';
import {RemoteService} from '../Services/remote.service';

@Component({
    selector: 'app-list',
    templateUrl: 'list.page.html',
    styleUrls: ['list.page.scss']
})
export class ListPage implements OnInit {
    connectionInfo;
    subscription: Subscription;

    constructor(public alertController: AlertController, private connService: ConnectionService,private remoto: RemoteService) {
        this.connectionInfo = this.connService.info;
        this.subscription = this.connService.getConnectionInfo().subscribe(info => {
            this.connectionInfo = info;
        });
    }

    ngOnInit() {
    }

    ngOnDestroy() {
        this.subscription.unsubscribe();
    }

    // add back when alpha.4 is out
    // navigate(item) {
    //   this.router.navigate(['/list', JSON.stringify(item)]);
    // }
    async cleanStorage() {
        sessionStorage.clear();
        this.connService.resetConnInfo();
        const alert = await this.alertController.create({
            header: 'Configuración restablecida correctamente',
            buttons: ['Aceptar']
        });
        await alert.present();
    }

    async conectarARemoto() {
        const alert = await this.alertController.create({
            header: 'Establecer Conexion',
            message: 'Inserte la URL/IP del modulo remoto',
            inputs: [
                {
                    label: 'URL',
                    name: 'url',
                    type: 'text',
                    placeholder: 'http://esp8266.local',
                }
            ],
            buttons: [
                {
                    text: 'Cancelar',
                    role: 'cancel',
                    cssClass: 'secondary',
                }, {
                    text: 'Aceptar',
                    handler: resp => {

                        if (resp.url.length > 0) {
                            if (resp.url.indexOf('http') < 0) {
                                resp.url = 'http://' + resp.url;
                            }
                            this.connectionInfo.serverUrl = resp.url;
                        }
                        this.connService.updateConnectionInfo(this.connectionInfo);
                    }
                }
            ]
        });

        await alert.present();
    }

    setConnected() {
        this.connService.setConnectionStatus(!this.connectionInfo.connected);
    }

    reiniciarRemoto() {
        this.remoto.resetDevice().subscribe();
        this.connService.setConnectionStatus(false);
    }
}
