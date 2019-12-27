import {Component} from '@angular/core';
import {ConnectionService} from '../Services/connection.service';
import {Subscription} from 'rxjs';
import {RemoteService} from '../Services/remote.service';
import {DEFAULT_CONN_INFO, OUTPUTS} from '../../environments/environment';
import {Storage} from '@ionic/storage';
import {LoadingController} from '@ionic/angular';

@Component({
    selector: 'app-home',
    templateUrl: 'home.page.html',
    styleUrls: ['home.page.scss'],
})
export class HomePage {

    inputs: Array<any> = OUTPUTS;

    icon = 'radio-button-on';
    icon_enable = 'success';
    icon_disabled = 'danger';
    edit: Boolean = false;
    info: any;
    subscription: Subscription;

    constructor(private connService: ConnectionService,
                private remoteService: RemoteService,
                private storage: Storage,
                public loadingController: LoadingController) {
        this.info = DEFAULT_CONN_INFO;
        this.connService.info().then(res => {
            if (res) {
                this.info = res;
            }
            this.getFromServer();
        });
        this.subscription = this.connService.getConnectionInfo().subscribe(inf => {
            this.info = inf;
        });

    }

    ngOnDestroy() {
        this.subscription.unsubscribe();
    }

    getFromServer() {
        this.storage.get('outputs').then(outputs => {
            if (outputs) {
                this.inputs = outputs;
            }
            this.remoteService.getStatus().subscribe(resp => {
                    this.connService.setConnectionStatus(true);
                    this.inputs.forEach(inp => {
                        inp.enable = false;
                        if (resp['outputs'][inp.alias] !== undefined) {
                            inp.enable = true;
                            inp.state = resp['outputs'][inp.alias] === 'true' ? true : false;
                        }
                    });
                    this.updateOutputs();
                }, error => {
                    this.updateOutputs();
                    this.connService.setConnectionStatus(false);
                }
            );
        });
    }

    changeState(input) {
        input.state = !input.state;
        this.remoteService.setOutput(input.alias, input.state).subscribe(res => {
            this.updateOutputs();
        });
    }

    updateOutputs() {
        this.storage.set('outputs', this.inputs);
    }
}
