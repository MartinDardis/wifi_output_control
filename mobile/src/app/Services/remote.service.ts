import {Injectable} from '@angular/core';
import {HttpClient} from '@angular/common/http';
import {ConnectionService} from './connection.service';

@Injectable({
    providedIn: 'root'
})
export class RemoteService {

    connInfo;

    constructor(private http: HttpClient, private connService: ConnectionService) {
        this.connInfo = connService.info;
        this.connService.getConnectionInfo().subscribe(resp => {
            this.connInfo = resp;
        });
    }

    getStatus() {
        const url = this.connInfo.serverUrl + '/status';
        return this.http.get(url);
    }

    setOutput(name, state) {
        const url = this.connInfo.serverUrl + '/status?' + name + '=' + (state ? 'ON' : 'OFF');
        return this.http.get(url);
    }

    resetDevice() {
        const url = this.connInfo.serverUrl + '/wifi.html?reset=do';
        return this.http.get(url);
    }
}
