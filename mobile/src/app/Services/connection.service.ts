import {Injectable} from '@angular/core';
import {DEFAULT_CONN_INFO} from '../../environments/environment';
import {Subject} from 'rxjs';
import {Storage} from '@ionic/storage';

@Injectable({
    providedIn: 'root'
})
export class ConnectionService {

    connInfoObs: Subject<any> = new Subject();

    constructor(private storage: Storage) {
        this.connInfoObs.subscribe(info => {
            this.storage.set('connectionInfo', info);
        });
    }

    info() {
       return this.storage.get('connectionInfo');
    }
    getConnectionInfo() {
        return this.connInfoObs.asObservable();
    }

    updateConnectionInfo(info) {
        this.connInfoObs.next(info);
    }

    isConnected() {
        return this.storage.get('connectionInfo').then(info => {
            if (!info) {
                return false;
            }
            return info.connected;
        });
    }

    setConnectionStatus(b: boolean) {
        this.storage.get('connectionInfo').then(infoSaved => {
            if (infoSaved === null) {
                infoSaved = DEFAULT_CONN_INFO;
            }
            infoSaved.connected = b;
            this.connInfoObs.next(infoSaved);
        });
    }

    resetConnInfo() {
        this.connInfoObs.next(DEFAULT_CONN_INFO);
    }
}
