module Promise = {
  /**
   * A simple functional Promise pipe. It takes an array of functions that return Promises to run in
   * sequence. It returns a function that takes an initial input, passes it to the first function in
   * the array, awaits the results, then passes them on to the next function in the array, and so on.
   */
  [@genType]
  let pipe = (functions, input) =>
    functions
    |> Js.Array.reduce(
         (promise, func) => promise |> Js.Promise.then_(func),
         Js.Promise.resolve(input),
       );
};
