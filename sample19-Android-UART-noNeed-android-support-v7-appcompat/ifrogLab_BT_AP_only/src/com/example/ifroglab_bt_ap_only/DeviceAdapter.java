package com.example.ifroglab_bt_ap_only;

import java.util.List;
import java.util.Map;


import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

class DeviceAdapter extends BaseAdapter {
    Context context;
    List<BluetoothDevice> devices;
    LayoutInflater inflater;
    Map<String, Integer> devRssiValues;

    public DeviceAdapter(Context context, List<BluetoothDevice> devices,Map<String, Integer> idevRssiValues) {
        this.context = context;
        inflater = LayoutInflater.from(context);
        this.devices = devices;
        this.devRssiValues=idevRssiValues;
    }
    
    @Override
    public int getCount() {
        return devices.size();
    }
    
    @Override
    public Object getItem(int position) {
        return devices.get(position);
    }
    
    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewGroup vg;

        if (convertView != null) {
            vg = (ViewGroup) convertView;
        } else {
            vg = (ViewGroup) inflater.inflate(R.layout.device_element, null);
        }

        BluetoothDevice device = devices.get(position);
        final TextView tvadd = ((TextView) vg.findViewById(R.id.address));
        final TextView tvname = ((TextView) vg.findViewById(R.id.name));
        final TextView tvpaired = (TextView) vg.findViewById(R.id.paired);
        final TextView tvrssi = (TextView) vg.findViewById(R.id.rssi);

        tvrssi.setVisibility(View.VISIBLE);
        byte rssival = (byte) devRssiValues.get(device.getAddress()).intValue();
        if (rssival != 0) {
            tvrssi.setText("Rssi = " + String.valueOf(rssival));
        }

        ///////
        tvname.setText(device.getName());
        tvadd.setText(device.getAddress());

            tvname.setTextColor(Color.WHITE);
            tvadd.setTextColor(Color.WHITE);
            tvpaired.setTextColor(Color.GRAY);
            tvpaired.setVisibility(View.VISIBLE);
            tvpaired.setText(R.string.paired);
            tvrssi.setVisibility(View.VISIBLE);
            tvrssi.setTextColor(Color.WHITE);
        
            
        return vg;
    }
}
