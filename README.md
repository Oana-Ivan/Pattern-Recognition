# Pattern-Recognition
---

This project identifies handwritten digits in an image and puts a colored border on the identified digit according to the value of the digit.

The idea of the project is to find small parts of an image that match a template image.

The image used was:

![pp2](https://user-images.githubusercontent.com/49486605/75514680-b87e3480-5a00-11ea-96a8-e2b36b52bc09.PNG)

The following images were used as templates:

![pp1](https://user-images.githubusercontent.com/49486605/75514546-5c1b1500-5a00-11ea-8d66-f3a19b453e20.PNG)

## Steps:

### 1. Template matching algorithm
- The images are converted to grayscales using the next formula for each pixel:

         R' = G' = B' = 0.299 ∗ R + 0.587 ∗ G + 0.114 ∗ B;
       
![pp3](https://user-images.githubusercontent.com/49486605/75515176-0e071100-5a02-11ea-9701-02d0340d0435.PNG)

- Each template slides onto the image by centering the template at each point of the image. For each position, the cross correlation is calculated. If the correlation is greater than a given value, then the border is drawn.
         
         The formula for the cross-correlation:    
     ![pp4](https://user-images.githubusercontent.com/49486605/75515871-ced9bf80-5a03-11ea-89ce-e7cf3f21e563.PNG)

### 2. Overlays
- Calculation of overlays

![pp6](https://user-images.githubusercontent.com/49486605/75516460-380e0280-5a05-11ea-9ca0-67a069e619f5.PNG)

- Elimination of overlays

![pp7](https://user-images.githubusercontent.com/49486605/75516453-35131200-5a05-11ea-9562-1e156b508435.PNG)
