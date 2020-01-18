package com.example.uidesign
import android.os.Bundle

import android.widget.TextView
import org.json.JSONObject
import androidx.appcompat.app.AppCompatActivity

import java.io.IOException
//import com.google.gson.GsonBuilder
import okhttp3.*
//import com.android.volley.toolbox.Volley
import okhttp3.Request
import okhttp3.Response




//import gson.*


class MainActivity:AppCompatActivity() {

    override fun onCreate(savedInstanceState:Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        fetchData()
    }

    private fun fetchData() {
        println("OHHHHHHHHH")

        val url = "https://api.thingspeak.com/channels/895784/feeds/last.json?api_key=PZNHSF28IWAHJ4Z7"

        val request = Request.Builder().url(url).build()

        val client = OkHttpClient()
        client.newCall(request).enqueue(object: Callback {
            override fun onResponse(call: Call, response: Response) {
                val body = response.body?.string()
                println(body)
                //val gson = GsonBuilder().create()
                //val homefeed = gson.fromJson(body, HomeFeed::class.java)

                runOnUiThread {
                    val t1 = findViewById<TextView>(R.id.textView)
                    val t2 = findViewById<TextView>(R.id.textView1)
                    val t3 = findViewById<TextView>(R.id.textView2)
                    val t4 = findViewById<TextView>(R.id.textView3)

                    val json = JSONObject(body.toString())
                    val f1 = json.getString("field1")
                    val f2 = json.getString("field2")
                    val f3 = json.getString("field3")
                    val f4 = json.getString("field4")

                    t1.text = f1
                    t2.text = f2
                    t3.text = f3
                }
            }
            override fun onFailure(call: Call, e: IOException) {
                println("failed to execute request")
            }
        })

    }
}

//class HomeFeed(val feeds: List<Feeds>){

//}


//class Feeds(val field1: Float, val field2: Float, val field3: Float){
//}
