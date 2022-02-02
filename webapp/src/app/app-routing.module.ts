import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { TestMapsComponent } from './test-maps/test-maps.component';

const routes: Routes = [
  { path: "", component: TestMapsComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
