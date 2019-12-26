import {Injectable} from '@angular/core';
import {DEFAULT_CONN_INFO} from '../../environments/environment';
import {Subject} from 'rxjs';
import {Storage} from '@ionic/storage';

@Injectable({
    providedIn: 'root'
})
export class ConnectionService {

    connInfoObs: Subject<any> = new Subject();
    info;

    constructor(private storage: Storage) {
        this.info = DEFAULT_CONN_INFO;
        this.storage.get('connectionInfo').then(infoSaved => {
            console.log(infoSaved);
            if (infoSaved) {
                this.info = JSON.parse(infoSaved);
            }
        });
    }

    getConnectionInfo() {
        this.connInfoObs.next(this.info);
        return this.connInfoObs.asObservable();
    }

    updateConnectionInfo(info) {
        this.storage.set('connectionInfo', JSON.stringify(info));
        this.info = info;
        this.connInfoObs.next(this.info);
    }

    isConnected() {
        return this.storage.get('connectionInfo').then(info => {
            if (!info) {
                return false;
            }
            return JSON.parse(info).connected;
        });
    }

    setConnectionStatus(b: boolean) {
        this.info.connected = b;
        this.connInfoObs.next(this.info);
    }

    resetConnInfo() {
        this.storage.set('connectionInfo', JSON.stringify(DEFAULT_CONN_INFO));
        this.connInfoObs.next(DEFAULT_CONN_INFO);
    }
}
