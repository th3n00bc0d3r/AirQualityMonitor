import { Component, OnInit } from '@angular/core';

@Component({
  selector: 'app-test-maps',
  templateUrl: './test-maps.component.html',
  styleUrls: ['./test-maps.component.scss']
})
export class TestMapsComponent implements OnInit {

  constructor() { }

  ngOnInit(): void {
  }

  options: google.maps.MapOptions = {
    center: {lat: 40, lng: -20},
    zoom: 4
  };

  center = {lat: 37.774546, lng: -122.433523};
  zoom = 12;
  heatmapOptions = {radius: 15};
  heatmapData = [
    {lat: 37.782, lng: -122.447, weight: 0.5},
    {lat: 37.782, lng: -122.445, weight: 1},
    {lat: 37.785, lng: -122.437, weight: 1},
    {lat: 37.785, lng: -122.435, weight: 1}
  ];  

}
