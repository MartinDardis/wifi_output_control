import {Component} from '@angular/core';
import {ConnectionService} from '../Services/connection.service';
import {Subscription} from 'rxjs';
import {RemoteService} from '../Services/remote.service';

@Component({
    selector: 'app-home',
    templateUrl: 'home.page.html',
    styleUrls: ['home.page.scss'],
})
export class HomePage {

    inputs: Array<any> = [
        {
            name: 'GPIO 1',
            alias: 'gpio0',
            state: false,
            enable: false,
        },
        {
            name: 'GPIO 2',
            alias: 'gpio1',
            state: false,
            enable: false,
        },
        {
            name: 'GPIO 3',
            alias: 'gpio2',
            state: false,
            enable: false,
        },
        {
            name: 'GPIO 4',
            alias: 'gpio3',
            state: false,
            enable: false,
        }
    ];

    icon = 'radio-button-on';
    icon_enable = 'success';
    icon_disabled = 'danger';
    edit: Boolean = false;
    info: any;
    subscription: Subscription;

    constructor(private connService: ConnectionService,
                private remoteService: RemoteService) {
        this.info = this.connService.info;
        this.subscription = this.connService.getConnectionInfo().subscribe(inf => {
            this.info = inf;
        });
        this.remoteService.getStatus().subscribe(resp => {
                this.connService.setConnectionStatus(true);
                this.inputs.forEach(inp => {
                    inp.enable = false;
                    if (resp['outputs'][inp.alias] !== undefined) {
                        inp.enable = true;
                        inp.state = resp['outputs'][inp.alias] === 'true' ? true : false;
                    }
                });
            }, error => {
                this.connService.setConnectionStatus(false);
            }
        );
    }

    ngOnDestroy() {
        this.subscription.unsubscribe();
    }

    changeState(input) {
        input.state = !input.state;
        this.remoteService.setOutput(input.alias, input.state).subscribe(res => {
            console.log(res);
        });
    }
}
