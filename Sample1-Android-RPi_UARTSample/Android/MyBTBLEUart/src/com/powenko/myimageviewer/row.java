package com.powenko.myimageviewer;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class row  extends BaseAdapter  {
	
	
	private final Context context;
	private final String[] values;
	private final String[] values2;

	private LayoutInflater mInflater;

	public row(Context context, String[] values, String[] values2) {
		mInflater = LayoutInflater.from(context);
		this.context = context;
	    this.values = values;
	    this.values2 = values2;
	}

	 

	@Override
	public int getCount() {
		return values.length;
	}

	@Override
	public Object getItem(int position) {
		return position;
	}

	@Override
	public long getItemId(int position) {
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		ViewHolder holder;
		if(convertView==null){
			convertView=mInflater.inflate(R.layout.rowlayout,null);
			holder = new ViewHolder();

            holder.imageView1 = (ImageView) convertView
                    .findViewById(R.id.imageView1 );
            holder.textView1 = (TextView) convertView
                    .findViewById(R.id.textView1 );
            holder.textView2 = (TextView) convertView
                    .findViewById(R.id.textView2);

            convertView.setTag(holder);
		}else{
			holder = (ViewHolder) convertView.getTag();
		}
			holder.imageView1.setImageResource(R.drawable.ic_launcher );   // 換圖片
                                                                           // Change the picture 
		
		holder.textView1.setText(values[position]);
		holder.textView2.setText(values2[position]);
		return convertView;
	}
	
	static class ViewHolder {
        ImageView imageView1;
        TextView textView1;
	    TextView textView2;
	}
}
